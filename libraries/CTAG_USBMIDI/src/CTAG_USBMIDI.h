/**
 * @file CTAG_USBMIDI.h
 * @brief A callback-based library for handling USB MIDI Host functionality.
 *
 * @defgroup Libraries_USBMIDI CTAG_USBMIDI
 * @ingroup Libraries
 * 
 * This library wraps the complexity of the Adafruit_TinyUSB library to provide
 * a simple, consistent API for sending and receiving USB MIDI messages.
 * 
 * 1. CTAG_USBMIDI
 */
#ifndef CTAG_USBMIDI_H
#define CTAG_USBMIDI_H

#include <Arduino.h>
#include "Adafruit_TinyUSB.h"

// --- Type Definitions for Callback Functions ---
typedef void (*NoteOnCallback)(byte channel, byte note, byte velocity);
typedef void (*NoteOffCallback)(byte channel, byte note, byte velocity);
typedef void (*ControlChangeCallback)(byte channel, byte controller, byte value);
typedef void (*ProgramChangeCallback)(byte channel, byte program);
typedef void (*PitchBendCallback)(byte channel, int value);

/**
 * @class CTAG_USBMIDI
 * @brief Handles sending and receiving MIDI messages via the USB Host port.
 */
class CTAG_USBMIDI {
public:
  CTAG_USBMIDI();

  /**
   * @brief Initializes the USB MIDI handler.
   */
  void begin();

  // --- PUBLIC HANDLERS FOR TINYUSB CALLBACKS ---
  /**
   * @brief Informs the library that a USB MIDI device has been connected.
   * @note Call this from your global `tuh_midi_mount_cb()` callback.
   * @param dev_addr The device address provided by TinyUSB.
   */
  void onDeviceMounted(uint8_t dev_addr);

  /**
   * @brief Informs the library that a USB MIDI device has been disconnected.
   * @note Call this from your global `tuh_midi_umount_cb()` callback.
   * @param dev_addr The device address provided by TinyUSB.
   */
  void onDeviceUnmounted(uint8_t dev_addr);

  /**
   * @brief Processes incoming MIDI packets from the USB stack.
   * @note Call this from your global `tuh_midi_rx_cb()` callback.
   * @param dev_addr The device address of the incoming packet.
   */
  void readPacket(uint8_t dev_addr);

  /**
   * @brief Checks if a MIDI device is currently connected and mounted.
   * @return True if a device is active, false otherwise.
   */
  bool isDeviceConnected() const;

  /**
   * @brief Gets the address of the currently connected MIDI device.
   * @return The device address (1-127) or 0 if no device is connected.
   */
  uint8_t getDeviceAddress() const;


  // --- SENDING MIDI MESSAGES ---
  void sendNoteOn(byte note, byte velocity, byte channel);
  void sendNoteOff(byte note, byte velocity, byte channel);
  void sendControlChange(byte controller, byte value, byte channel);
  void sendProgramChange(byte program, byte channel);
  void sendPitchBend(int value, byte channel);

  // --- ASSIGNING CALLBACKS ---
  void setHandleNoteOn(NoteOnCallback fptr);
  void setHandleNoteOff(NoteOffCallback fptr);
  void setHandleControlChange(ControlChangeCallback fptr);
  void setHandleProgramChange(ProgramChangeCallback fptr);
  void setHandlePitchBend(PitchBendCallback fptr);

  // --- UTILITY HELPER FUNCTIONS ---
  /**
   * @brief Converts a MIDI note number (0-127) into a readable name (e.g., "C#4").
   * @note This function is 'static', so it can be called without an object instance.
   */
  static String getNoteName(byte noteNumber);

private:
  // --- Internal Implementation Details ---
  void parse(uint8_t b);
  byte status;
  byte data1;
  byte data2;
  uint8_t _dev_addr; // Stores the address of the currently active MIDI device.

  // --- Callback Function Pointers ---
  NoteOnCallback handleNoteOn;
  NoteOffCallback handleNoteOff;
  ControlChangeCallback handleControlChange;
  ProgramChangeCallback handleProgramChange;
  PitchBendCallback handlePitchBend;
};

#endif // CTAG_USBMIDI_H