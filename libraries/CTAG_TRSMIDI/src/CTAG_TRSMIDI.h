/**
 * @file CTAG_TRSMIDI.h
 * @brief A callback-based library for handling MIDI over serial (TRS) connections.
 *
 * @defgroup Libraries_TRSMIDI CTAG_TRSMIDI
 * @ingroup Libraries
 * 
 * This library provides a simple, callback-based interface for sending and
 * receiving MIDI messages through a hardware serial port, abstracting away
 * the byte-level parsing of the MIDI protocol.
 * 
 * 1. CTAG_TRSMIDI
 */
#ifndef CTAG_TRSMIDI_h
#define CTAG_TRSMIDI_h

#include <Arduino.h>

// --- Type Definitions for Callback Functions ---
/** @brief Callback function pointer for Note On messages. */
typedef void (*NoteOnCallback)(byte channel, byte note, byte velocity);

/** @brief Callback function pointer for Note Off messages. */
typedef void (*NoteOffCallback)(byte channel, byte note, byte velocity);

/** @brief Callback function pointer for Control Change messages. */
typedef void (*ControlChangeCallback)(byte channel, byte controller, byte value);

/** @brief Callback function pointer for Program Change messages. */
typedef void (*ProgramChangeCallback)(byte channel, byte program);

/** @brief Callback function pointer for Pitch Bend messages. Pitch is a 14-bit value. */
typedef void (*PitchBendCallback)(byte channel, int value);

/**
 * @class CTAG_TRSMIDI
 * @brief Handles sending and receiving MIDI messages via a serial (TRS) port.
 */
class CTAG_TRSMIDI {
public:
  /**
   * @brief Constructor for the CTAG_TRSMIDI class.
   */
  CTAG_TRSMIDI();

  /**
   * @brief Initializes the library with a specific serial stream.
   * @param port A reference to the hardware serial stream (e.g., Serial1, Serial2).
   */
  void begin(Stream& port);

  /**
   * @brief Reads and processes incoming MIDI data from the serial port.
   * @note This function should be called repeatedly in your main loop to receive MIDI.
   */
  void read();

  // --- SENDING MIDI MESSAGES ---

  /**
   * @brief Sends a MIDI Note On message.
   * @param note The MIDI note number (0-127).
   * @param velocity The note velocity (0-127).
   * @param channel The MIDI channel (1-16).
   */
  void sendNoteOn(byte note, byte velocity, byte channel);

  /**
   * @brief Sends a MIDI Note Off message.
   * @param note The MIDI note number (0-127).
   * @param velocity The note velocity (0-127).
   * @param channel The MIDI channel (1-16).
   */
  void sendNoteOff(byte note, byte velocity, byte channel);

  /**
   * @brief Sends a MIDI Control Change message.
   * @param controller The controller number (0-127).
   * @param value The controller value (0-127).
   * @param channel The MIDI channel (1-16).
   */
  void sendControlChange(byte controller, byte value, byte channel);

  /**
   * @brief Sends a MIDI Program Change message.
   * @param program The program number (0-127).
   * @param channel The MIDI channel (1-16).
   */
  void sendProgramChange(byte program, byte channel);

  /**
   * @brief Sends a MIDI Pitch Bend message.
   * @param value The pitch bend value, from -8192 (full bend down) to 8191 (full bend up). 0 is center.
   * @param channel The MIDI channel (1-16).
   */
  void sendPitchBend(int value, byte channel);

  // --- ASSIGNING CALLBACKS ---

  /**
   * @brief Assigns a function to be called when a Note On message is received.
   * @param fptr A pointer to the function to be called.
   */
  void setHandleNoteOn(NoteOnCallback fptr);

  /**
   * @brief Assigns a function to be called when a Note Off message is received.
   * @param fptr A pointer to the function to be called.
   */
  void setHandleNoteOff(NoteOffCallback fptr);

  /**
   * @brief Assigns a function to be called when a Control Change message is received.
   * @param fptr A pointer to the function to be called.
   */
  void setHandleControlChange(ControlChangeCallback fptr);

  /**
   * @brief Assigns a function to be called when a Program Change message is received.
   * @param fptr A pointer to the function to be called.
   */
  void setHandleProgramChange(ProgramChangeCallback fptr);

  /**
   * @brief Assigns a function to be called when a Pitch Bend message is received.
   * @param fptr A pointer to the function to be called.
   */
  void setHandlePitchBend(PitchBendCallback fptr);


  // --- UTILITY HELPER FUNCTIONS ---

  /**
   * @brief Converts a MIDI note number (0-127) into a readable name (e.g., "C#4").
   * @param noteNumber The MIDI note number to convert.
   * @return A String object containing the note name.
   * @note This function is 'static', so it can be called without an object instance, e.g., `CTAG_TRSMIDI::getNoteName(60);`
   */
  static String getNoteName(byte noteNumber);

private:
  // --- Internal Implementation Details ---
  Stream* midiPort;
  void parse(byte midiByte);
  byte status;
  byte data1;
  byte data2;

  // --- Callback Function Pointers ---
  NoteOnCallback handleNoteOn;
  NoteOffCallback handleNoteOff;
  ControlChangeCallback handleControlChange;
  ProgramChangeCallback handleProgramChange;
  PitchBendCallback handlePitchBend;
};

#endif // CTAG_TRSMIDI_h