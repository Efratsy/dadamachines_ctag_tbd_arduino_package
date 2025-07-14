/**
 * 
 * @file SimpleSynth.ino
 * @brief Demonstration sketch for the CTAG Audio Library.
 *
 * @defgroup Examples_Audio SimpleSynth
 * @ingroup Examples
 * 
 * This SimpleSynth.ino example shows how to:
 * 1. Initialize the audio codec.
 * 2. Start the audio engine.
 * 3. Create a sine wave oscillator (VCO) and set it as the audio source.
 * 4. Control the oscillator's frequency in the main loop to create a siren effect.
 * 
 */

// Include the custom board pin definitions. This is crucial!
#include "pins_arduino.h"

// Include the main library header.
#include "CTAG_Audio.h"

// --- Global Objects ---

/**
 * @brief Global instance of the audio codec driver.
 */
CTAG_AudioCodec codec;

/**
 * @brief Global instance of our sine wave oscillator "plugin".
 */
CTAG_VCO_Sine myVCO;


/**
 * @brief Audio Task that get
 */
void audioTask(void *pvParameters) {
  delay(125);

  // --- bring up the codec ---
  if (!codec.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL)) {
    Serial.println("Codec initialization failed! Halting.");
    while (1);
  }
  codec.setHeadphoneVolume(70);
  Serial.println("Codec initialized successfully.");

  // --- configure the I2S engine (no loop yet) ---
  CTAG_AudioEngine::init(I2S_NUM_0);

  // --- hook up your VCO and defaults ---
  CTAG_AudioEngine::setSource(&myVCO);
  myVCO.setFrequency(220.0f);
  myVCO.setAmplitude(0.5f);

  Serial.println("Starting Audio Engine...");

  // loop pumping audio ---
  for (;;) {
    CTAG_AudioEngine::renderBlock();

    float freq = 220.0f + 220.0f * sin( millis() * 0.001f );
    myVCO.setFrequency(freq);
  }
}



/**
 * @brief Runs once at startup to initialize the hardware and software.
 */
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- CTAG Audio Framework Demo ---");

  xTaskCreatePinnedToCore(audioTask, "AudioTask", 4096, NULL, 2, NULL, 1);
}
/**
 * @brief Runs continuously after setup().
 * The audio is generated in a background task, so this loop is free for controls.
 */
void loop() {
}