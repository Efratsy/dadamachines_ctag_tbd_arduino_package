/**
 * @file pins_arduino.h
 * @brief Pin definitions for the Dadamachines CTAG-TBD **RP2350 (UI-MCU)**
 *
 * @defgroup CTAG_TBD_RP2350_Variant  CTAG-TBD RP2350 Variant
 * @ingroup  Variants
 *
 * This header exposes every board-specific pin used by the Arduino core.
 * Keep macro *names* stable so that earlier sketches build unmodified,
 * but point them to the new GPIOs found in the latest EasyEDA schematic.
 *
 * <b>MCU:</b> RP2350 REV B
 */

#pragma once
#include <stdint.h>

/* -------------------------------------------------------------------------
 *  Silicon revision selector
 *  ---------------------------------------------------------------------- */
/** @brief RP2350 silicon revision (0 = rev B, 1 = rev A) */
#define PICO_RP2350A 0

/* ------------------------------------------------------------------------
 *  LEDs & NeoPixel
 * ----------------------------------------------------------------------*/
/** On-board green status LED (label “LED2 SZYY0603G1”). */
#define PIN_LED              (24u)

/** Data-in for the chain of 16 NeoPixels on the extension board. */
#define PIN_NEOPIXELS        (26u)   ///< @anchor PIN_NEOPIXELS

/* ------------------------------------------------------------------------
 *  UART — Debug + two independent MIDI I/O pairs
 * ----------------------------------------------------------------------*/
/**
 * Debug/CDC console (USB-serial mirror possible).
 * - **TX:** GPIO0  (label “RP2XXX_DEBUG_TX”)
 * - **RX:** GPIO1  (label “– (‘Green X’)”)
 */
#define PIN_SERIAL1_TX       (0u)
#define PIN_SERIAL1_RX       (1u)

/**
 * Hardware MIDI Port #1 (TRS Type-A)
 * - **TX (OUT):** GPIO36 (label “RP2XXX_MIDI_OUT1”)
 * - **RX (IN):**  GPIO37 (label “RP2XXX_MIDI_IN1”)
 */
#define PIN_SERIAL2_TX       (36u)
#define PIN_SERIAL2_RX       (37u)

/**
 * Optional second TRS-MIDI pair (not used in the original examples).
 * - **TX (OUT):** GPIO44 (label “RP2XXX_MIDI_OUT2”)
 * - **RX (IN):**  GPIO45 (label “RP2XXX_MIDI_IN2”)
 */
#define PIN_SERIAL3_TX       (44u)
#define PIN_SERIAL3_RX       (45u)

/* Standard Arduino aliases (debug port). */
static const uint8_t TX = PIN_SERIAL1_TX;
static const uint8_t RX = PIN_SERIAL1_RX;

/* ------------------------------------------------------------------------
 *  SPI  – communication with the ESP32-P4
 * ----------------------------------------------------------------------*/
/**
 * IPC link <b>SPI-0 (HS)</b> – 10 MHz, high-speed path.  
 *   MISO → GPIO32   (label “RP2XXX-ESP32_MISO_2”)  
 *   MOSI → GPIO35   (label “RP2XXX-ESP32_MOSI_2”)  
 *   SCK  → GPIO34   (label “RP2XXX-ESP32_SCLK_2”)  
 *   CS   → GPIO33   (label “RP2XXX-ESP32_CS_2”)
 */
#define PIN_SPI0_MISO        (32u)
#define PIN_SPI0_MOSI        (35u)
#define PIN_SPI0_SCK         (34u)
#define PIN_SPI0_SS          (33u)

/**
 * Secondary IPC / fallback <b>SPI-1</b> (used only in specialised tests).  
 *   MISO → GPIO28   (label “RP2XXX-ESP32_MISO_1”)  
 *   MOSI → GPIO31   (label “RP2XXX-ESP32_MOSI_1”)  
 *   SCK  → GPIO30   (label “RP2XXX-ESP32_SCLK_1”)  
 *   CS   → GPIO29   (label “RP2XXX-ESP32_CS_1”)
 */
#define PIN_SPI1_MISO        (28u)
#define PIN_SPI1_MOSI        (31u)
#define PIN_SPI1_SCK         (30u)
#define PIN_SPI1_SS          (29u)

/* ------------------------------------------------------------------------
 *  I²C   – UI-bus + Codec-bus
 * ----------------------------------------------------------------------*/
/** I²C-0 (UI bus) – Buttons/Encoders via STM32 on the extension board. */
#define PIN_WIRE0_SDA        (20u)
#define PIN_WIRE0_SCL        (21u)

/** I²C-1 (Codec + misc) – mirrored on the main board header. */
#define PIN_WIRE1_SDA        (38u)
#define PIN_WIRE1_SCL        (39u)

/* Arduino-core counts. */
#define SERIAL_HOWMANY       (3u)
#define SPI_HOWMANY          (2u)
#define WIRE_HOWMANY         (2u)

/* ------------------------------------------------------------------------
 *  Misc control / power / test pins
 * ----------------------------------------------------------------------*/
#define PIN_USB_PWR_FAULT    (9u)   ///< USB power-fault input (active-low)
#define PIN_USB_PWR_EN       (10u)  ///< Enables 5 V to USB-A host port
#define PIN_USB_SEL          (11u)  ///< FSUSB42 MUX select: Host ↔ Device
#define PIN_FSUSB42MUX_SEL   (11u)  ///< Legacy alias for backward-compatibility
#define PIN_BOOTSEL_BUTTON   (3u)   ///< BOOT/RESET button routed to RP2350

/* ------------------------------------------------------------------------
 *  ADC inputs
 * ----------------------------------------------------------------------*/
#define PIN_GPIO40_ADC0      (40u)  ///< Free ADC0
#define PIN_GPIO41_ADC1      (41u)  ///< Free ADC1
#define PIN_GPIO42_ADC2      (42u)  ///< Free ADC2
#define PIN_GPIO43_ADC3      (43u)  ///< Free ADC3 (shares MIDI OUT2)
#define PIN_GPIO44_ADC4      (44u)  ///< MIDI OUT2  (also analog in)
#define PIN_GPIO45_ADC5      (45u)  ///< MIDI IN2   (also analog in)
#define PIN_GPIO46_ADC6      (46u)  ///< Free ADC6
#define PIN_GPIO47_ADC7      (47u)  ///< Free ADC7

/* ------------------------------------------------------------------------
 *  Bring in Philhower-core generic helpers
 * ----------------------------------------------------------------------*/
#include "../generic/common.h"
