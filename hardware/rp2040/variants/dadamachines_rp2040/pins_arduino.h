/**
 * @file pins_arduino.h
 * @brief Pin definitions for the Dadamachines CTAG TBD board.
 * 
 * @defgroup CTAG_TBD_RP2040_Variant CTAG TBD RP2040 Variant
 * @ingroup Variants
 *
 * This header file contains all pin assignments for the specific
 * RP2040-based board. The definitions are intended for use with the Arduino core.
 *
 * 1. dadamachines_rp2040/pins_arduino.h
 */


#pragma once

// Dadamachines CTAG TBD RP2040 UI MCU pin definitions

/**
 * @name LED Pins
 * @{
 */
#define PIN_LED          (25u) ///< IO25: Onboard LED (SZYY0603G1)
/** @} */

/**
 * @name Serial Interfaces (UART)
 * @{
 */
#define PIN_SERIAL1_TX   (0u)  ///< IO0: Debug UART TX (RP2040_DEBUG_TX)
#define PIN_SERIAL1_RX   (1u)  ///< IO1: Debug UART RX (RP2040_DEBUG_RX)

#define PIN_SERIAL2_TX   (8u)  ///< IO8: MIDI-Out (RP2040_MIDI_OUT)
#define PIN_SERIAL2_RX   (9u)  ///< IO9: MIDI-In (RP2040_MIDI_IN)
/** @} */

/**
 * @name SPI Interface (SPI0)
 * @brief Pins used for communication with the ESP32.
 * @{
 */
#define PIN_SPI0_MISO    (20u) ///< IO20: MISO for ESP32 (RP2040_ESP32_MISO)
#define PIN_SPI0_MOSI    (19u) ///< IO19: MOSI for ESP32 (RP2040_ESP32_MOSI)
#define PIN_SPI0_SCK     (18u) ///< IO18: SCLK for ESP32 (RP2040_ESP32_SCLK)
#define PIN_SPI0_SS      (21u) ///< IO21: Chip Select for ESP32 (RP2040_ESP32_CS)
/** @} */

/**
 * @name I2C Interfaces (Wire)
 * @{
 */
#define PIN_WIRE0_SDA    (16u) ///< IO16: I2C0 SDA (RP2040_SDA0)
#define PIN_WIRE0_SCL    (17u) ///< IO17: I2C0 SCL (RP2040_SCL0)

#define PIN_WIRE1_SDA    (26u) ///< IO26: I2C1 SDA (RP2040_SDA1)
#define PIN_WIRE1_SCL    (27u) ///< IO27: I2C1 SCL (RP2040_SCL1)
/** @} */

/**
 * @name I2S Interface
 * @{
 */
#define PIN_ESP32_I2S_WS (22u) ///< IO22: Word Select for ESP32 I2S (ESP32_I2S_WS)
/** @} */


/**
 * @name Control and System Pins
 * @{
 */
#define PIN_USB_PWR_FAULT    (2u)  ///< IO2: USB power fault indicator
#define PIN_RP2040_BOOT      (3u)  ///< IO3: Boot control pin for RP2040
#define PIN_RP2040_CLK_IN    (4u)  ///< IO4: External clock input for RP2040
#define PIN_FSUSB42MUX_SEL   (5u)  ///< IO5: Selector pin for FSUSB42MUX
#define PIN_NEOPIXELS        (12u) ///< IO12: Data line for NeoPixels (RP2040_NEOPIXELS3V)
#define PIN_USB_PWR_EN       (24u) ///< IO24: USB power enable pin
/** @} */

/**
 * @name ADC Pins (Analog-to-Digital Converter)
 * @{
 */
#define PIN_GPIO28_ADC2      (28u) ///< IO28: ADC2 input
/** @} */

/**
 * @name General Purpose GPIO Pins
 * @brief Pins available for general use.
 * @{
 */
#define PIN_GPIO6            (6u)  ///< IO6: General-purpose GPIO
#define PIN_GPIO7            (7u)  ///< IO7: General-purpose GPIO
#define PIN_GPIO10           (10u) ///< IO10: General-purpose GPIO
#define PIN_GPIO11           (11u) ///< IO11: General-purpose GPIO
#define PIN_GPIO13           (13u) ///< IO13: General-purpose GPIO
#define PIN_GPIO14           (14u) ///< IO14: General-purpose GPIO
#define PIN_GPIO15           (15u) ///< IO15: General-purpose GPIO
#define PIN_GPIO23           (23u) ///< IO23: General-purpose GPIO
#define PIN_GPIO29           (29u) ///< IO29: General-purpose GPIO
/** @} */

/**
 * @name Debug Interface (SWD)
 * @brief Pins used for Serial Wire Debug.
 * @note These pins are typically reserved for debugging and may not have standard Arduino GPIO numbers.
 * @{
 */
// #define PIN_SWCLK       (??) ///< SWCLK: Serial Wire Clock
// #define PIN_SWDAT       (??) ///< SWDAT: Serial Wire Data (aka SWDIO)
/** @} */

/**
 * @name Number of Available Peripherals
 * @{
 */
#define SERIAL_HOWMANY (2u)
#define SPI_HOWMANY    (1u)
#define WIRE_HOWMANY   (2u)
/** @} */

// Include generic definitions provided by the core
#include "../generic/common.h"
