/**
 * @file pins_arduino.h
 * @brief Pin definitions for the **Dadamachines CTAG-TBD – ESP32-P4** variant.
 *
 * @defgroup CTAG_TBD_ESP32P4_Variant CTAG TBD ESP32-P4 Variant
 * @ingroup Variants
 *
 * This header exposes all board-specific GPIO assignments so that
 * sketches written for the original ESP32-S3 version compile
 * unmodified on the ESP32-P4 evaluation hardware.
 *
 * Naming conventions follow the earlier S3 file:
 *   • Upper-case macro for every functional pin (e.g. `PIN_I2S_BCLK`)
 *   • Standard Arduino aliases (`LED_BUILTIN`, `MOSI`, …)
 */

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

/* ----------------------------------------------------------------------
 *  USB identification
 * -------------------------------------------------------------------- */
/** @name USB identifiers */
///@{
#define USB_VID 0x303aU          ///< Espressif USB Vendor ID
#define USB_PID 0x1001U          ///< Generic ESP32 device Product ID
///@}

/* ----------------------------------------------------------------------
 *  Status LED / Neopixel
 * -------------------------------------------------------------------- */
/** @name LED / Neopixel */
///@{
#define PIN_NEOPIXELS   (6u)     ///< GPIO6 : data line for on-board NeoPixel
static const uint8_t LED_BUILTIN = PIN_NEOPIXELS;
#define BUILTIN_LED LED_BUILTIN
///@}

/* ----------------------------------------------------------------------
 *  UART interfaces
 * -------------------------------------------------------------------- */
/** @name UART 0 – serial console */
///@{
#define PIN_SERIAL1_TX  (37u)    ///< TXD0 : GPIO37 – USB-to-UART bridge TX
#define PIN_SERIAL1_RX  (38u)    ///< RXD0 : GPIO38 – USB-to-UART bridge RX
///@}

/** @name UART 2 – TRS MIDI */
///@{
#define PIN_SERIAL2_TX  (8u)     ///< GPIO8  – MIDI TX (unchanged)
#define PIN_SERIAL2_RX  (11u)    ///< GPIO11 – MIDI RX (unchanged)
///@}

/* Arduino core aliases */
static const uint8_t TX = PIN_SERIAL1_TX;
static const uint8_t RX = PIN_SERIAL1_RX;

/* ----------------------------------------------------------------------
 *  I²C buses
 * -------------------------------------------------------------------- */
/** @name I²C-0  (OLED, generic) */
///@{
#define PIN_WIRE0_SDA   (5u)     ///< GPIO5  – SDA
#define PIN_WIRE0_SCL   (4u)     ///< GPIO4  – SCL
///@}

/** @name I²C-1  (Audio codec) */
///@{
#define PIN_WIRE1_SDA   (7u)     ///< GPIO7  – ESP32_CODEC_SDA
#define PIN_WIRE1_SCL   (8u)     ///< GPIO8  – ESP32_CODEC_SCL
///@}

/* Standard Arduino aliases → default to Wire0 */
static const uint8_t SDA = PIN_WIRE0_SDA;
static const uint8_t SCL = PIN_WIRE0_SCL;

/* ----------------------------------------------------------------------
 *  SPI-3  (RP2350 ↔ ESP32-P4 „IPC“ link)
 * -------------------------------------------------------------------- */
/** @name SPI-3 : RP2350 ↔ ESP32 IPC */
///@{
#define PIN_SPI0_MISO   (22u)    ///< GPIO22 – RP2350-ESP32 MISO (channel 2)
#define PIN_SPI0_MOSI   (23u)    ///< GPIO23 – RP2350-ESP32 MOSI (channel 2)
#define PIN_SPI0_SCK    (21u)    ///< GPIO21 – RP2350-ESP32 SCLK (channel 2)
#define PIN_SPI0_SS     (20u)    ///< GPIO20 – RP2350-ESP32 CS   (channel 2)
///@}

/* ----------------------------------------------------------------------
 *  SPI-2  (RP2350 ↔ ESP32-P4 „IPC“ link)
 * -------------------------------------------------------------------- */
/** @name SPI-2 : RP2350 ↔ ESP32 IPC */
///@{
#define PIN_SPI1_MISO   (29u)    ///< GPIO29 – RP2350-ESP32 MISO (channel 1)
#define PIN_SPI1_MOSI   (31u)    ///< GPIO31 – RP2350-ESP32 MOSI (channel 1)
#define PIN_SPI1_SCK    (30u)    ///< GPIO30 – RP2350-ESP32 SCLK (channel 1)
#define PIN_SPI1_SS     (28u)    ///< GPIO28 – RP2350-ESP32 CS   (channel 1)
///@}

/* Arduino aliases */
static const uint8_t MOSI = PIN_SPI0_MOSI;
static const uint8_t MISO = PIN_SPI0_MISO;
static const uint8_t SCK  = PIN_SPI0_SCK;
static const uint8_t SS   = PIN_SPI0_SS;

/* ----------------------------------------------------------------------
 *  I²S audio interface (TLV320AIC3254)
 * -------------------------------------------------------------------- */
/** @name I²S codec lines */
///@{
#define PIN_I2S_MCLK    (13u)    ///< GPIO13 – Master Clock
#define PIN_I2S_BCLK    (12u)    ///< GPIO12 – Bit Clock
#define PIN_I2S_WS      (10u)    ///< GPIO10 – Word Select / LRCLK
#define PIN_I2S_SDOUT   (11u)    ///< GPIO11 – Serial Data OUT
#define PIN_I2S_SDIN    (9u)     ///< GPIO9  – Serial Data IN
///@}

/* ----------------------------------------------------------------------
 *  Boot-mode & user-switch pins
 * -------------------------------------------------------------------- */
#define PIN_BOOT_SEL    (0u)     ///< GPIO0  – Boot-mode select (held at reset)
#define PIN_UI_SWITCH   (2u)     ///< GPIO2  – User push-button

/* ----------------------------------------------------------------------
 *  USB HS interface
 * -------------------------------------------------------------------- */
#define PIN_USB_DN      (19u)    ///< GPIO19 – USB D-
#define PIN_USB_DP      (20u)    ///< GPIO20 – USB D+  (shared with SS)

/* ----------------------------------------------------------------------
 *  Spare GPIOs (documented for user projects)
 * -------------------------------------------------------------------- */
#define PIN_GPIO14      (14u)
#define PIN_GPIO15      (15u)
#define PIN_GPIO16      (16u)
#define PIN_GPIO17      (17u)
/* … add more if needed */

#endif /* Pins_Arduino_h */
