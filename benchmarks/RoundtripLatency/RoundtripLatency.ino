/**
 * @file RoundtripLatency.ino
 * @brief Measures the audio roundtrip latency internally using micros().
 *
 * This sketch sends an audio impulse and measures the time it takes to
 * receive it back on the line-in, reporting the latency in microseconds
 * on the Serial Monitor.
 */

#include <Arduino.h>
#include <Wire.h>
#include "pins_arduino.h"
#include "CTAG_Audio.h"
#include "driver/i2s.h"

// --- Measurement Configuration ---
#define IMPULSE_THRESHOLD 15000 // The audio level to detect the returning impulse.

// --- Global Objects & State ---
CTAG_AudioCodec codec;
volatile bool trigger_impulse = false;

// --- Latency Measurement Variables ---
volatile unsigned long impulse_start_time_us = 0;
volatile unsigned long roundtrip_latency_us = 0;
volatile bool new_latency_measured = false;

/**
 * @brief The dedicated audio task running on Core 1.
 * It handles both sending the test impulse and reading the audio input.
 */
void audioTask(void *pvParameters) {
  const int buffer_frames = 256;
  int16_t* tx_buffer = (int16_t*)malloc(buffer_frames * 2 * sizeof(int16_t));
  int16_t* rx_buffer = (int16_t*)malloc(buffer_frames * 2 * sizeof(int16_t));

  for (;;) {
    // 1. Prepare the Transmit (TX) Buffer
    memset(tx_buffer, 0, buffer_frames * 2 * sizeof(int16_t));

    if (trigger_impulse) {
      trigger_impulse = false;

      // Create a short, loud impulse
      for (int i = 0; i < 4; ++i) {
        tx_buffer[i*2]     = 32767;
        tx_buffer[i*2 + 1] = 32767;
      }
      
      // RECORD START TIME
      impulse_start_time_us = micros();
    }

    // 2. Perform I2S Read/Write
    size_t bytes_read, bytes_written;
    i2s_write(I2S_NUM_0, tx_buffer, buffer_frames * 2 * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    i2s_read(I2S_NUM_0, rx_buffer, buffer_frames * 2 * sizeof(int16_t), &bytes_read, portMAX_DELAY);

    // 3. Analyze the Receive (RX) Buffer
    if (impulse_start_time_us > 0) { // Only check for an echo if an impulse was sent
      for (int i = 0; i < buffer_frames * 2; ++i) {
        if (abs(rx_buffer[i]) > IMPULSE_THRESHOLD) {
          // RECORD END TIME & CALCULATE LATENCY
          unsigned long end_time_us = micros();
          roundtrip_latency_us = end_time_us - impulse_start_time_us;
          
          // Reset start time to prevent re-triggering on the same impulse echo
          impulse_start_time_us = 0; 
          
          // Set flag for the main loop to print the result
          new_latency_measured = true; 
          break; 
        }
      }
    }
  }
}

// ====================================================================================
//                                      SETUP
// ====================================================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- CTAG Audio Roundtrip Latency (Software Test) ---");

  // Initialize the Audio Codec
  if (!codec.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL)) {
    Serial.println("Codec init FAIL! Halting.");
    while (1);
  }
  
  // Activate the Line-In path WITH gain
  codec.enableLineIn();
  
  codec.setHeadphoneVolume(80);
  Serial.println("Codec initialized for IN and OUT.");

  // Manually configure the I2S driver for full-duplex (TX and RX)
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
      .sample_rate = 48000, .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, .dma_buf_count = 8, .dma_buf_len = 256
  };
  i2s_pin_config_t pin_config = {
      .mck_io_num = PIN_I2S_MCLK, .bck_io_num = PIN_I2S_BCLK,
      .ws_io_num = PIN_I2S_WS, .data_out_num = PIN_I2S_SDOUT,
      .data_in_num = PIN_I2S_SDIN
  };
  
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  Serial.println("I2S Driver configured for TX & RX.");

  // Create and pin the audio task to Core 1
  xTaskCreatePinnedToCore(audioTask, "AudioTask", 4096, nullptr, 2, nullptr, 1);
  Serial.println("Test running...");
}

// ====================================================================================
//                                       LOOP
// ====================================================================================
void loop() {
  // Trigger a new test impulse every 2 seconds
  trigger_impulse = true;
  Serial.println("Triggering new impulse...");
  
  // Check if the audio task has reported a new measurement
  if (new_latency_measured) {
    new_latency_measured = false; // Reset the flag
    
    Serial.print("--> Latency Detected: ");
    Serial.print(roundtrip_latency_us);
    Serial.println(" microseconds");
    Serial.println("------------------------------------");
  }
  
  delay(2000);
}