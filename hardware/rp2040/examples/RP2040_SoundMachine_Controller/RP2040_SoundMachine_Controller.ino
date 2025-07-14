/**
 * @file RP2040_SoundMachine_Controller.ino
 * @brief Core sketch for the RP2040 acting as the main UI controller.
 *
 * This sketch performs the following tasks:
 * - Core 0: Reads all inputs from the CTAG Extension Board, and sends the complete UI state via SPI to a sound engine (ESP32).
 * - Core 1: Acts as a dedicated UI renderer, displaying the state on the OLED
 * and providing visual feedback on the LEDs.
 */
// --- CTAG Libraries ---
#include <CTAG_ExtensionBoard.h>
#include <CTAG_Display.h>
#include <CTAG_SPI_IPC.h>

// --- System & Hardware Headers ---
#include "pins_arduino.h"
#include <SPI.h>

// ====================================================================================
//                             GLOBAL OBJECTS & STATE
// ====================================================================================
// --- Pin Definitions ---
// Use the standardized pin names from your board's "pins_arduino.h" file.
#define PIN_MISO PIN_SPI0_MISO
#define PIN_MOSI PIN_SPI0_MOSI
#define PIN_SCK  PIN_SPI0_SCK
#define PIN_CS   PIN_SPI0_SS

// --- Library Objects ---
CTAG_ExtensionBoard extBoard(17, PIN_NEOPIXELS, 0x42, &Wire1);
CTAG_Display        display;

// --- Inter-Core Communication Mailboxes (written by Core 1, read by Core 0) ---
// These volatile variables safely pass data from the UI core to the comms core.
volatile int      pot_value_A = 0;
volatile int      pot_value_B = 0;
volatile int      pot_value_C = 0;
volatile int      pot_value_D = 0;
volatile int      encoder_pos = 0;

/**
 * @brief This structure defines the data packet sent via SPI to the ESP32.
 */
typedef struct {
  uint16_t pot_values[4];   // Current values of the 4 pots (0-1023)
  uint8_t  encoder_pos;     // Current absolute position of the encoder
} ctag_ipc_packet_t;

// ====================================================================================
//                                  SETUP (CORE 0)
// ====================================================================================
void setup() {
  Serial1.begin(115200);

  // Configure the SPI pins for the RP2040.
  SPI.setMISO(PIN_MISO);
  SPI.setMOSI(PIN_MOSI);
  SPI.setSCK(PIN_SCK);
  
  // Activate the SPI hardware in master mode.
  // `false` means the CS pin will be controlled manually by the library.
  SPI.begin(false);


  // Initialize the SPI library in Master Mode for communication with the ESP32
  CTAG_SPI_IPC::beginMaster(PIN_SPI0_SS, SPI);
}

// ====================================================================================
//                                  SETUP (CORE 1)
// ====================================================================================
void setup1() {
  // Initialize all User Interface hardware
  extBoard.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL);
  display.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL, 0x3C);
  extBoard.setBrightness(20);
}

// ====================================================================================
//                            LOOP (CORE 0) - Comms Core
// ====================================================================================
void loop() {
  // This core's only job is to read the latest UI state from the mailboxes
  // and send it over SPI.
  ctag_ipc_packet_t packet_to_send;

  // Read the mailbox variables into the packet structure
  packet_to_send.pot_values[0] = pot_value_A;
  packet_to_send.pot_values[1] = pot_value_B;
  packet_to_send.pot_values[2] = pot_value_C;
  packet_to_send.pot_values[3] = pot_value_D;
  packet_to_send.encoder_pos   = encoder_pos;
  
  // --- SERIAL PRINT FOR DEBUGGING ---
  // This block prints the content of the packet right before it's sent.
  // It only prints when a value has changed to avoid flooding the monitor.
  static ctag_ipc_packet_t last_sent_packet;
  if (memcmp(&packet_to_send, &last_sent_packet, sizeof(ctag_ipc_packet_t)) != 0) {
    Serial1.println("----------------------------------------");
    Serial1.println("CORE 0: Change detected. Preparing to send SPI packet...");
    
    // Print each value of the packet individually
    Serial.printf("  - Pot Values: A=%-4d, B=%-4d, C=%-4d, D=%-4d\n", 
                    packet_to_send.pot_values[0], packet_to_send.pot_values[1], 
                    packet_to_send.pot_values[2], packet_to_send.pot_values[3]);
    Serial.printf("  - Encoder Pos: %d\n", packet_to_send.encoder_pos);
    
    // Update the last sent packet state, so we only print on change
    last_sent_packet = packet_to_send;
  }

  // Send the data packet to the ESP32
  bool ok = CTAG_SPI_IPC::send((const uint8_t*)&packet_to_send, sizeof(packet_to_send));
  if (!ok) {
    Serial1.println("CORE 0: ERROR - SPI send failed!");
  }

  // Send updates roughly 60 times per second
  delay(16);
}

// ====================================================================================
//                         LOOP (CORE 1) - User Interface Core
// ====================================================================================
void loop1() {
  static uint16_t raw[4] = {0};
  if (extBoard.update()) {
    // Read raw pot values
    raw[0] = extBoard.getPot(0);
    raw[1] = extBoard.getPot(1);
    raw[2] = extBoard.getPot(2);
    raw[3] = extBoard.getPot(3);
    uint8_t enc = extBoard.getEncoderAbsolutePosition();

    // Clamp encoder to [0..3]
    encoder_pos = enc < 4 ? enc : (uint8_t)(enc % 4);

    // Map pot ranges based on synth type
    switch (encoder_pos) {
      case 0: // Sine VCO
        pot_value_A = map(raw[0], 0, 1023,  20, 3000);   // Frequency Hz
        pot_value_B = map(raw[1], 0, 1023,   0,  100);   // Amplitude ×100
        pot_value_C = map(raw[2], 0, 1023,   1,   10);   // LFO Rate Hz
        pot_value_D = map(raw[3], 0, 1023,   0,  500);   // LFO Depth cents
        break;
      case 1: // Square VCO
        pot_value_A = map(raw[0], 0, 1023,  20, 3000);   // Frequency Hz
        pot_value_B = map(raw[1], 0, 1023,   0,  100);   // Amplitude ×100
        pot_value_C = map(raw[2], 0, 1023,   5,  95);   // Duty Cycle
        pot_value_D = map(raw[3], 0, 1023,   0,  500);   // UNUSED
        break;
      case 2: // Saw VCO
        pot_value_A = map(raw[0], 0, 1023,  20, 3000);   // Frequency Hz
        pot_value_B = map(raw[1], 0, 1023,   0,  100);   // Amplitude ×100
        pot_value_C = map(raw[2], 0, 1023,   1,   99);   // LFO Rate Hz
        pot_value_D = map(raw[3], 0, 1023,   0,  500);   // UNUSED
        break;
      case 3: // Wavetable
        pot_value_A = map(raw[0], 0, 1023,  20, 3000);   // Carrier Frequency (Hz)
        pot_value_B = map(raw[1], 0, 1023,   0,  100);   // Amplitude ×100
        pot_value_C = map(raw[2], 0, 1023,  20, 3000);   // Modulator Frequency (Hz)
        pot_value_D = map(raw[3], 0, 1023,   0, 25);   // Modulation Index (depth)
        break;
    }
  }

  // UI Rendering
  display.writeRow(0, "CTAG Synth Controller");

  // Synth names
  const char* names[4] = {
    "Sine VCO",
    "Square VCO",
    "Saw VCO",
    "2-Freq FMSynth"
  };
  display.writeRow(2, names[encoder_pos]);

  // Parameter labels per synth
  const char* labels[4][4] = {
    {"Freq(Hz)",  "Amp     ", "LFO Rate", "LFO Dpth"},
    {"Freq(Hz)","Amp     ","Duty Cycl","Unused  "},
    {"Freq(Hz)","Amp     ","Skew     ","Unused  "},
    {"Carr(Hz)","Amp     ","Mod (Hz)","Mod Dpth"},
  };

  // Display each label + its mapped value
  char buf[22];
  snprintf(buf, sizeof(buf), "%s: %-4d", labels[encoder_pos][0], pot_value_A);
  display.writeRow(4, buf);
  snprintf(buf, sizeof(buf), "%s: %-4d", labels[encoder_pos][1], pot_value_B);
  display.writeRow(5, buf);
  snprintf(buf, sizeof(buf), "%s: %-4d", labels[encoder_pos][2], pot_value_C);
  display.writeRow(6, buf);
  snprintf(buf, sizeof(buf), "%s: %-4d", labels[encoder_pos][3], pot_value_D);
  display.writeRow(7, buf);

  // Push to screen and LEDs
  display.display();
  extBoard.show();

  // ~30 FPS UI
  delay(33);
}