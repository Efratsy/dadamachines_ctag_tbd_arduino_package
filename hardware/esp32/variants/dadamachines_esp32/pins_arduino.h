/**
 * @file pins_arduino.h
 * @brief Pin definitions for the Dadamachines CTAG TBD board.
 * 
 * @defgroup CTAG_TBD_ESP32_Variant CTAG TBD ESP32 Variant
 * @ingroup Variants
 *
 * This header file contains all pin assignments for the specific
 * ESP32-based board. The definitions are intended for use with the Arduino core.
 *
 * 1. dadamachines_esp32/pins_arduino.h
 */

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

/**
 * @name USB Identification
 * @{
 */
#define USB_VID (0x303aU) ///< Vendor ID für Espressif
#define USB_PID (0x1001U) ///< Product ID für generische ESP32-S3 Boards
/** @} */

/**
 * @name LED Pins
 * @{
 */
#define PIN_NEOPIXELS   (38u) ///< IO38: Data line for NeoPixels

// Standard Arduino API LED alias (maps to NeoPixel pin)
static const uint8_t LED_BUILTIN = PIN_NEOPIXELS;
#define BUILTIN_LED LED_BUILTIN
#define LED_BUILTIN LED_BUILTIN
/** @} */

/**
 * @name Serial Interfaces (UART)
 * @{
 */
// Main UART (Debug/Monitor)
#define PIN_SERIAL1_TX  (43u) ///< TXD0: Main UART transmit pin (ESP32_UART-TXD)
#define PIN_SERIAL1_RX  (44u) ///< RXD0: Main UART receive pin (ESP32_UART-RXD)

// MIDI interface
#define PIN_SERIAL2_TX  (8u)  ///< IO8: MIDI transmit pin (ESP32_IO8_MIDITX)
#define PIN_SERIAL2_RX  (11u) ///< IO11: MIDI receive pin (ESP32_MIDIRIX)

// Standard Arduino aliases
static const uint8_t TX = PIN_SERIAL1_TX;
static const uint8_t RX = PIN_SERIAL1_RX;
/** @} */

/**
 * @name I2C Interfaces (Wire)
 * @{
 */
// I2C Bus 0 (e.g., for OLED display)
#define PIN_WIRE0_SDA   (5u)  ///< IO5: I2C SDA for OLED display
#define PIN_WIRE0_SCL   (4u)  ///< IO4: I2C SCL for OLED display

// I2C Bus 1 (e.g., for audio codec)
#define PIN_WIRE1_SDA   (41u) ///< IO41: I2C SDA for audio codec
#define PIN_WIRE1_SCL   (40u) ///< IO40: I2C SCL for audio codec

// Standard Arduino aliases (refer to Wire0)
static const uint8_t SDA = PIN_WIRE0_SDA;
static const uint8_t SCL = PIN_WIRE0_SCL;
/** @} */

/**
 * @name SPI Interface (SPI0)
 * @brief Pins for communication with the RP2040.
 * @{
 */
#define PIN_SPI0_MISO   (16u) ///< IO16: MISO for RP2040 (RP2040_ESP32_MISO)
#define PIN_SPI0_MOSI   (17u) ///< IO17: MOSI for RP2040 (RP2040_ESP32_MOSI)
#define PIN_SPI0_SCK    (15u) ///< IO15: SCLK for RP2040 (RP2040_ESP32_SCLK)
#define PIN_SPI0_SS     (18u) ///< IO18: Chip Select for RP2040 (RP2040_ESP32_CS)

// Standard Arduino aliases
static const uint8_t SS   = PIN_SPI0_SS;
static const uint8_t MOSI = PIN_SPI0_MOSI;
static const uint8_t MISO = PIN_SPI0_MISO;
static const uint8_t SCK  = PIN_SPI0_SCK;
/** @} */

/**
 * @name I2S Interface
 * @{
 */
#define PIN_I2S_MCLK    (39u) ///< IO39: Master Clock (ESP32_I2S_MCLK)
#define PIN_I2S_BCLK    (45u) ///< IO45: Bit Clock (ESP32_I2S_BCLK)
#define PIN_I2S_WS      (1u)  ///< IO1: Word Select (Left/Right Clock) (ESP32_I2S_WS)
#define PIN_I2S_SDOUT   (2u)  ///< IO2: Serial Data Out (ESP32_I2S_SDOUT)
#define PIN_I2S_SDIN    (21u) ///< IO21: Serial Data In (ESP32_I2S_SDIN)
/** @} */

/**
 * @name Control and System Pins
 * @{
 */
#define PIN_BOOT_SEL    (0u)  ///< IO0: Boot mode selection (ESP32_BOOT_SEL)
#define PIN_UI_SWITCH   (6u)  ///< IO6: User interface switch (ESP32_UI-SW)
/** @} */

/**
 * @name Onboard USB
 * @{
 */
#define PIN_USB_DN      (19u) ///< IO19: USB D-
#define PIN_USB_DP      (20u) ///< IO20: USB D+
/** @} */

/**
 * @name General Purpose GPIO Pins
 * @brief Pins available for general use.
 * @{
 */
#define PIN_GPIO7       (7u)  ///< IO7
#define PIN_GPIO9       (9u)  ///< IO9
#define PIN_GPIO10      (10u) ///< IO10
#define PIN_GPIO12      (12u) ///< IO12
#define PIN_GPIO13      (13u) ///< IO13
#define PIN_GPIO14      (14u) ///< IO14
#define PIN_GPIO42      (42u) ///< IO42
/** @} */

#endif /* Pins_Arduino_h */