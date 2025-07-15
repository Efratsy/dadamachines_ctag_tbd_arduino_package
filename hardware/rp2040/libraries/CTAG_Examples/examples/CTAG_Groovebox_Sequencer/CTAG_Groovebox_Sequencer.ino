/**
 * @file CTAG_Groovebox_Sequencer.ino
 * @brief A 16-step MIDI sequencer using the CTAG hardware platform on the RP2040.
 */

// --- High-Level CTAG Libraries ---
#include <CTAG_ExtensionBoard.h>
#include <CTAG_Display.h>
#include <CTAG_TRSMIDI.h>
#include <CTAG_USBMIDI.h>

// --- Required Low-Level and System Headers ---
#include "pins_arduino.h"
#include "Adafruit_TinyUSB.h"
#include <usb_midi_host.h>

// ====================================================================================
//                             GLOBAL OBJECTS & STATE
// ====================================================================================

// --- Library Objects ---
CTAG_ExtensionBoard extBoard(17, PIN_NEOPIXELS, 0x42, &Wire1);
CTAG_Display        display;
CTAG_TRSMIDI        trsMidi;
CTAG_USBMIDI        usbMidi;
Adafruit_USBH_Host  USBHost;

// --- Sequencer State ---
uint16_t sequence_pattern = 0b0000000000000000;
int      current_step     = 0;
bool     is_playing       = true;

// --- Inter-Core Communication Mailboxes ---
volatile int  param_tempo_bpm = 120;
volatile int  param_note      = 60;
volatile int  param_velocity  = 100;
volatile int  param_gate_ms   = 100;

// ====================================================================================
//                                  SETUP (CORE 0)
// ====================================================================================
void setup() {
  // --- Configure board for USB Host Mode ---
  pinMode(PIN_USB_PWR_EN, OUTPUT);
  digitalWrite(PIN_USB_PWR_EN, HIGH);
  pinMode(PIN_FSUSB42MUX_SEL, OUTPUT);
  digitalWrite(PIN_FSUSB42MUX_SEL, HIGH);
  delay(200);

  // Initialize the low-level USB Host stack
  USBHost.begin(0);
	
  Serial2.begin(31250);
	
  // Initialize MIDI libraries
  trsMidi.begin(Serial2);
  usbMidi.begin();
}

// ====================================================================================
//                                  SETUP (CORE 1)
// ====================================================================================
void setup1() {
  // Initialize all user interface components on Core 1
  extBoard.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL);
  display.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL, 0x3C);
  extBoard.setBrightness(30);
}

// ====================================================================================
//                                   LOOP (CORE 0) - The "Brain"
// ====================================================================================
void loop() {
  // Handle USB events
  USBHost.task();

  // --- Sequencer Timing Logic ---
  static unsigned long last_step_time = 0;
  static unsigned long note_on_time = 0;
  static bool note_is_on = false;
  
  unsigned long step_interval_ms = 60000 / param_tempo_bpm / 4; // 16th notes

  // Check if it's time for the next step
  if (is_playing && (millis() - last_step_time >= step_interval_ms)) {
    last_step_time = millis();

    // Check if the current step is active in the pattern
    if ((sequence_pattern >> current_step) & 1) {

      if (usbMidi.isDeviceConnected() && tuh_midi_configured(usbMidi.getDeviceAddress())) {
        usbMidi.sendNoteOn(param_note, param_velocity, 1);
      }

      trsMidi.sendNoteOn(param_note, param_velocity, 1);
      
      note_on_time = millis();
      note_is_on = true;
    }

    // Advance to the next step
    current_step = (current_step + 1) % 16;
  }

  // --- Handle Note Off based on Gate Length ---
  // Check if a note is currently playing and its duration has expired
  if (note_is_on && (millis() - note_on_time >= param_gate_ms)) {

    if (usbMidi.isDeviceConnected() && tuh_midi_configured(usbMidi.getDeviceAddress())) {
        usbMidi.sendNoteOff(param_note, 0, 1);
    }
    trsMidi.sendNoteOff(param_note, 0, 1);
    
    note_is_on = false; // Mark the note as off
  }
}

// ====================================================================================
//                                   LOOP (CORE 1) - The "User Interface"
// ====================================================================================
void loop1() {
  extBoard.update();

  // Read User Input & Update Parameters
  param_tempo_bpm = map(extBoard.getPot(0), 0, 1023, 40, 240);
  param_note      = map(extBoard.getPot(1), 0, 1023, 36, 84);
  param_velocity  = map(extBoard.getPot(2), 0, 1023, 1, 127);
  param_gate_ms   = map(extBoard.getPot(3), 0, 1023, 10, 500);

  for (int i = 0; i < 16; ++i) {
    
    if (extBoard.isButtonPressed(i)) {
      sequence_pattern ^= (1 << i);
      delay(50); // Simple debounce
    }
  }

  // --- Update Visuals (Display & LEDs) ---
  char buffer[22];
  display.writeRow(0, "CTAG Groovebox");
  snprintf(buffer, 22, "Tempo: %3d BPM", param_tempo_bpm);
  display.writeRow(2, buffer);
  snprintf(buffer, 22, "Note : %-3s (%d)", CTAG_USBMIDI::getNoteName(param_note).c_str(), param_note);
  display.writeRow(3, buffer);
  snprintf(buffer, 22, "Vel  : %3d", param_velocity);
  display.writeRow(4, buffer);
  snprintf(buffer, 22, "Gate : %3d ms", param_gate_ms);
  display.writeRow(5, buffer);


  for (int i = 0; i < 16; ++i) {
    if ((sequence_pattern >> i) & 1) {
      extBoard.setLed(i + 1, 0, 0, 255); // Blue for active steps
    } else {
      extBoard.setLed(i + 1, 0, 0, 0);   // Off for inactive steps
    }
  }
  if (is_playing) {
    extBoard.setLed(current_step + 1, 0, 255, 0); // Green for the running light
  }
  extBoard.show(); 

  delay(20);
}

// ====================================================================================
//                       TinyUSB Host Callback Functions
// ====================================================================================
void tuh_midi_mount_cb(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx) {
  usbMidi.onDeviceMounted(dev_addr);
}

void tuh_midi_umount_cb(uint8_t dev_addr, uint8_t instance) {
  usbMidi.onDeviceUnmounted(dev_addr);
}

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets) {
  usbMidi.readPacket(dev_addr);
}