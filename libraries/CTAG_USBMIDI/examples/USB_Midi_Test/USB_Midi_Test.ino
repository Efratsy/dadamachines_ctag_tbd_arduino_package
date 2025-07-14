/**
 * @file USB_Midi_Test.ino
 * @brief Final, corrected sketch using CTAG libraries for USB MIDI and Display.
 * 
 * @defgroup Examples_USBMIDI USB_Midi_Test
 * @ingroup Examples
 * 
 * 
 *  The USB_Midi_Test.ino example demonstrates both sending and receiving all common MIDI messages using,
 *  Adafruit_Tinyusb and a USB-MIDI-Connection.
 */

// --- High-Level Libraries ---
#include <CTAG_USBMIDI.h>
#include <CTAG_Display.h>

// --- Required Low-Level and System Headers ---
#include "pins_arduino.h"
#include "Adafruit_TinyUSB.h"
#include <usb_midi_host.h>

// --- Global Library Objects ---
CTAG_USBMIDI midi;
CTAG_Display display;
Adafruit_USBH_Host USBHost;

// --- Global State & Inter-Core Communication ("Mailboxes") ---
volatile char usb_midi_status_line[22] = "NOT CONNECTED";
volatile char note_status_out_line[22] = "OUT: --";
volatile char note_status_in_line[22]  = "IN: --";

// --- Application-Level MIDI Callbacks ---
void handleNoteOn(byte channel, byte note, byte velocity) {
  String noteName = CTAG_USBMIDI::getNoteName(note);
  snprintf((char*)note_status_in_line, 22, "IN: NoteOn %s V:%d", noteName.c_str(), velocity);
}
void handleNoteOff(byte channel, byte note, byte velocity) {
  String noteName = CTAG_USBMIDI::getNoteName(note);
  snprintf((char*)note_status_in_line, 22, "IN: NoteOff %s", noteName.c_str());
}

// ====================================================================================
//                                  SETUP (CORE 0)
// ====================================================================================
void setup() {
  // Configure board for USB Host Mode
  pinMode(PIN_USB_PWR_EN, OUTPUT);
  digitalWrite(PIN_USB_PWR_EN, HIGH);
  pinMode(PIN_FSUSB42MUX_SEL, OUTPUT);
  digitalWrite(PIN_FSUSB42MUX_SEL, HIGH);
  delay(200);

  // Initialize USB Host Stack and our MIDI library
  USBHost.begin(0);
  midi.begin();

  // Register callbacks
  midi.setHandleNoteOn(handleNoteOn);
  midi.setHandleNoteOff(handleNoteOff);
}

// ====================================================================================
//                                  SETUP (CORE 1)
// ====================================================================================
void setup1() {
  // Initialize the display
  display.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL, 0x3C);
  display.clear();
  display.writeRow(0, "CTAG MIDI System");
  display.writeRow(1, "---------------------");
}

// ====================================================================================
//                                   LOOP (CORE 0)
// ====================================================================================
void loop() {
  USBHost.task();

  // --- Automatic Send Test ---
  static unsigned long lastActionTime = 0;
  static int sendState = 0;
  const long NOTE_ON_INTERVAL = 2000;
  const long NOTE_DURATION = 200;

  if (millis() - lastActionTime > (sendState == 0 ? NOTE_ON_INTERVAL : NOTE_DURATION) ) {
    lastActionTime = millis();

    // === THE CRITICAL FIX IS HERE ===
    // Check if device is mounted AND fully configured before sending.
    if (midi.isDeviceConnected() && tuh_midi_configured(midi.getDeviceAddress())) {
      if (sendState == 0) {
        snprintf((char*)note_status_out_line, 22, "OUT: Note On C4");
        midi.sendNoteOn(60, 100, 1);
        sendState = 1;
      } else {
        snprintf((char*)note_status_out_line, 22, "OUT: --");
        midi.sendNoteOff(60, 0, 1);
        sendState = 0;
      }
    }
  }
}

// ====================================================================================
//                                   LOOP (CORE 1)
// ====================================================================================
void loop1() {
  display.writeRow(0, (const char*)usb_midi_status_line);
  display.writeRow(3, (const char*)note_status_out_line);
  display.writeRow(5, (const char*)note_status_in_line);
  delay(50);
}

// ====================================================================================
//                       TinyUSB Host Callback Functions
// ====================================================================================
void tuh_midi_mount_cb(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx) {
  snprintf((char*)usb_midi_status_line, 22, "MIDI CONNECTED");
  midi.onDeviceMounted(dev_addr);
}

void tuh_midi_umount_cb(uint8_t dev_addr, uint8_t instance) {
  snprintf((char*)usb_midi_status_line, 22, "NOT CONNECTED");
  snprintf((char*)note_status_out_line, 22, "OUT: --");
  snprintf((char*)note_status_in_line, 22, "IN: --");
  midi.onDeviceUnmounted(dev_addr);
}

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets) {
  midi.readPacket(dev_addr);
}