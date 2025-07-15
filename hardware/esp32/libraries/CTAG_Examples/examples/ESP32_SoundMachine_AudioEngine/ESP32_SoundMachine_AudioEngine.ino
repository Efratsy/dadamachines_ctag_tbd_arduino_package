/**
 * @file ESP32_SoundMachine_AudioEngine.ino
 * @brief A clean, dual-core sound engine for the ESP32 using FreeRTOS.
 *
 * This sketch implements the specified architecture:
 * - A dedicated task on Core 0 handles SPI communication and synth logic.
 * - A dedicated task on Core 1 handles real-time audio generation.
 * - A mutex ensures thread-safe data exchange between the tasks.
 */

// --- Libraries & Headers ---
#include <CTAG_Audio.h> // We only use the CTAG_AudioCodec class from here
#include <CTAG_SPI_IPC.h>
#include "pins_arduino.h"
#include "driver/i2s.h" // We need direct I2S access for our custom audio task
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"       // for IRAM_ATTR if you ever need it
#include "soc/rtc.h"        // for portMUX_TYPE
//#include <freertos/portmacro.h>


// ====================================================================================
//                             GLOBAL OBJECTS & STATE
// ====================================================================================

// --- Core Objects ---
CTAG_AudioCodec codec;          ///< I2C driver for the audio codec chip.
CTAG_VCO_Sine sineSynth;        ///< A simple sine wave synthesizer object.
CTAG_VCO_Square squareSynth;    ///< A simple square wave synthesizer object.
CTAG_VCO_Saw sawSynth;          ///< A simple saw wave synthesizer object.
CTAG_FMSynth fmSynth;           ///< A simple FM synthesizer object.


/** @brief A low-level spinlock for protecting shared data between cores and ISRs. */
static portMUX_TYPE dataMux = portMUX_INITIALIZER_UNLOCKED;

// --- IPC Data Structure (must match the RP2040) ---
typedef struct {
  uint16_t pot_values[4];
  uint8_t  encoder_pos;
} ctag_ipc_packet_t;

// --- Shared Resources ---
/** @brief The "mailbox" for SPI data, written by ISR, read by audioTask. Declared volatile. */
volatile ctag_ipc_packet_t received_data;
/** @brief Flag set by ISR to signal new data is ready for processing. Declared volatile. */
volatile bool newDataFromSPI = false;
/** @brief Flag to confirm the SPI slave has been initialized. Declared volatile. */
volatile bool slave_init = false;


// ====================================================================================
//                             CALLBACK & RTOS TASKS
// ====================================================================================
/** * @brief SPI ISR Callback. Triggered when a full data packet is received.
 * @note This function runs in an interrupt context and must be as fast as possible.
 * It uses a critical section (spinlock) for thread-safe data copy.
 */
void onSpiPacketReceived(const uint8_t* data, size_t len) {
  if (len == sizeof(ctag_ipc_packet_t)) {
    portENTER_CRITICAL(&dataMux);
    memcpy((void*)&received_data, data, len);
    newDataFromSPI = true;
    portEXIT_CRITICAL(&dataMux);
  }
}



/** * @brief The logic task running on Core 0. Its only job is to initialize the SPI slave.
 */
void logicTask(void *pvParameters) {
  if (CTAG_SPI_IPC::beginSlave(PIN_SPI0_SCK, PIN_SPI0_MISO, PIN_SPI0_MOSI, PIN_SPI0_SS, onSpiPacketReceived)) {
    Serial.println("Slave ready and waiting for packets...");
    slave_init = true;
  } else {
    Serial.println("Slave initialization FAILED!");
  }

  // The Loop has done its job and can be deleted to save resources.
  vTaskDelete(NULL);
}

/** * @brief The main audio task running on Core 1.
 * This task initializes all audio hardware and then enters an infinite loop
 * to process controller data and render audio blocks.
 */
void audioTask(void *pvParameters) {
  delay(125); // Small delay to ensure Core 0 can print its startup messages.

  // 1) Bring up the codec
  if (!codec.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL)) {
    Serial.println("Codec initialization failed! Halting Core 1.");
    vTaskDelay(pdMS_TO_TICKS(200));
  }
  codec.setHeadphoneVolume(70);
  Serial.println("Codec initialized successfully.");

  // 2) Configure the I2S engine (no render loop yet, just init hardware)
  CTAG_AudioEngine::init(I2S_NUM_0); 

  // 3) Clear any previous source
  CTAG_AudioEngine::setSource(nullptr);
  Serial.println("Audio Engine initialized. Starting render loop...");

  ctag_ipc_packet_t lastPacket = { { 0 }, 0 };
  for (;;) {
    ctag_ipc_packet_t localPacket;
    
    // Safely copy the latest data from the ISR's mailbox
    portENTER_CRITICAL(&dataMux);
      memcpy(&localPacket, (const void*)&received_data, sizeof(localPacket));
    portEXIT_CRITICAL(&dataMux);
    
    // Check if the data has changed since the last loop cycle
    if (memcmp(&localPacket, &lastPacket, sizeof(localPacket)) != 0) {
      
      // Use the encoder position to select the active synth object
      switch(localPacket.encoder_pos) {
        case 0: {// --- Sine VCO Slot ---
          CTAG_AudioEngine::setSource(&sineSynth);

          uint16_t frequency     = localPacket.pot_values[0];
          uint16_t amplitude    = localPacket.pot_values[1];
          uint16_t vibrato_rate  = localPacket.pot_values[2];
          uint16_t vibrato_depth = localPacket.pot_values[3];

          sineSynth.setFrequency(frequency);

          float normalised_amplitude = (float)amplitude / 100.0f;             
          sineSynth.setAmplitude(normalised_amplitude);

          sineSynth.setLfoRate(vibrato_rate);
          sineSynth.setLfoDepth(vibrato_depth);
          break;
        }

        case 1: {// --- Square VCO Slot ---
          CTAG_AudioEngine::setSource(&squareSynth);

          uint16_t frequency     = localPacket.pot_values[0];
          uint16_t amplitude     = localPacket.pot_values[1];
          uint16_t duty_cycle    = localPacket.pot_values[2];

          squareSynth.setFrequency(frequency);
          
          float normalised_amplitude = (float)amplitude / 100.0f;             
          squareSynth.setAmplitude(normalised_amplitude);
          
          float normalised_duty_cycle = (float)duty_cycle / 100.0f; 
          squareSynth.setDutyCycle(normalised_duty_cycle);
          break;
        }

        case 2: {// --- Saw VCO Slot ---
           CTAG_AudioEngine::setSource(&sawSynth);

           uint16_t frequency     = localPacket.pot_values[0];
           uint16_t amplitude     = localPacket.pot_values[1];
           uint16_t skew          = localPacket.pot_values[2];

           sawSynth.setFrequency(frequency);

           float normalised_amplitude = (float)amplitude / 100.0f;             
           sawSynth.setAmplitude(normalised_amplitude);

           float normalised_skew = (float)skew / 100.0f; 
           sawSynth.setSkew(normalised_skew);
           break;
        }

        case 3: {// --- FM Synth Slot ---
           CTAG_AudioEngine::setSource(&fmSynth);

           uint16_t carrier_frequency    = localPacket.pot_values[0];
           uint16_t amplitude            = localPacket.pot_values[1];
           uint16_t modular_frequency    = localPacket.pot_values[2];
           uint16_t modular_depth        = localPacket.pot_values[3];

           fmSynth.setCarrierFreq(carrier_frequency);
          
           float normalised_amplitude = (float)amplitude / 100.0f;             
           fmSynth.setAmplitude(normalised_amplitude);

           fmSynth.setModFreq(modular_frequency);
           fmSynth.setModIndex(modular_depth);
           break;
        }

        default: {
           CTAG_AudioEngine::setSource(nullptr); // No sound
           break;
        }
      }
      
      // Remember this packet for the next comparison
      memcpy(&lastPacket, &localPacket, sizeof(localPacket));
    }
    
    // 5) Render exactly one block of audio with the current settings
    CTAG_AudioEngine::renderBlock();
  }
}
// ====================================================================================
//                                       SETUP
// ====================================================================================
/**
 * @brief Main setup function. Its only job is to create the FreeRTOS tasks.
 */
void setup() {
  Serial.begin(115200);
  Serial.println("\n--- CTAG ESP32 Synth Engine ---");

  xTaskCreatePinnedToCore(logicTask, "LogicTask", 4096, NULL, 1, NULL, 0);
  delay(1000); // Give logic task time to initialize SPI
  xTaskCreatePinnedToCore(audioTask, "AudioTask", 8192, NULL, 2, NULL, 1); // Audio needs more stack and higher priority

  Serial.println("System setup complete. Tasks are running.");
}

// ====================================================================================
//                                        LOOP
// ====================================================================================
/**
 * @brief The main Arduino loop is not used in this RTOS-based sketch.
 */
void loop() {
  vTaskDelete(NULL);
}