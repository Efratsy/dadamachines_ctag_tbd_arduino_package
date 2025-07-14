/**
 * @file CTAG_USBMIDI.cpp
 * @brief Implementation file for the CTAG_USBMIDI library.
 */

#include "CTAG_USBMIDI.h"
#include <usb_midi_host.h>

// Array of note names for the getNoteName() helper function.
static const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

CTAG_USBMIDI::CTAG_USBMIDI() {
  // Initialize all callbacks to NULL
  handleNoteOn = NULL;
  handleNoteOff = NULL;
  handleControlChange = NULL;
  handleProgramChange = NULL;
  handlePitchBend = NULL;

  // Initialize the parser state machine
  status = 0;
  data1 = 0;
  data2 = 0;
  _dev_addr = 0; // No device connected initially
}

void CTAG_USBMIDI::begin() {
  _dev_addr = 0;
}

void CTAG_USBMIDI::onDeviceMounted(uint8_t dev_addr) {
  if (_dev_addr == 0) {
    _dev_addr = dev_addr;
  }
}

void CTAG_USBMIDI::onDeviceUnmounted(uint8_t dev_addr) {
  if (dev_addr == _dev_addr) {
    _dev_addr = 0;
  }
}

void CTAG_USBMIDI::readPacket(uint8_t dev_addr) {
  if (dev_addr != _dev_addr) return;

  uint8_t packet[4];
  while (tuh_midi_packet_read(dev_addr, packet)) {
    // The first byte contains the Code Index Number (CIN), which tells us the message type.
    uint8_t cin = packet[0] & 0x0F;
    if (cin > 1) { // Ignore system messages for now, but parse the MIDI bytes
      parse(packet[1]);
      if(packet[2] != 0) parse(packet[2]);
      if(packet[3] != 0) parse(packet[3]);
    }
  }
}

bool CTAG_USBMIDI::isDeviceConnected() const {
  return (_dev_addr != 0);
}

uint8_t CTAG_USBMIDI::getDeviceAddress() const {
  return _dev_addr;
}

// --- SENDING MIDI MESSAGES (using the packet API) ---

void CTAG_USBMIDI::sendNoteOn(byte note, byte velocity, byte channel) {
  if (_dev_addr == 0 || !tuh_midi_configured(_dev_addr)) return;
  uint8_t packet[4] = { 0x09, (uint8_t)(0x90 | (channel - 1)), note, velocity };
  tuh_midi_packet_write(_dev_addr, packet);
  tuh_midi_stream_flush(_dev_addr);
}

void CTAG_USBMIDI::sendNoteOff(byte note, byte velocity, byte channel) {
  if (_dev_addr == 0 || !tuh_midi_configured(_dev_addr)) return;
  uint8_t packet[4] = { 0x08, (uint8_t)(0x80 | (channel - 1)), note, velocity };
  tuh_midi_packet_write(_dev_addr, packet);
  tuh_midi_stream_flush(_dev_addr);
}

void CTAG_USBMIDI::sendControlChange(byte controller, byte value, byte channel) {
  if (_dev_addr == 0 || !tuh_midi_configured(_dev_addr)) return;
  uint8_t packet[4] = { 0x0B, (uint8_t)(0xB0 | (channel - 1)), controller, value };
  tuh_midi_packet_write(_dev_addr, packet);
  tuh_midi_stream_flush(_dev_addr);
}

void CTAG_USBMIDI::sendProgramChange(byte program, byte channel) {
  if (_dev_addr == 0 || !tuh_midi_configured(_dev_addr)) return;
  uint8_t packet[4] = { 0x0C, (uint8_t)(0xC0 | (channel - 1)), program, 0 };
  tuh_midi_packet_write(_dev_addr, packet);
  tuh_midi_stream_flush(_dev_addr);
}

void CTAG_USBMIDI::sendPitchBend(int value, byte channel) {
  if (_dev_addr == 0 || !tuh_midi_configured(_dev_addr)) return;
  unsigned int midiValue = value + 8192;
  byte lsb = midiValue & 0x7F;
  byte msb = (midiValue >> 7) & 0x7F;
  uint8_t packet[4] = { 0x0E, (uint8_t)(0xE0 | (channel - 1)), lsb, msb };
  tuh_midi_packet_write(_dev_addr, packet);
  tuh_midi_stream_flush(_dev_addr);
}


// --- ASSIGNING CALLBACKS ---
void CTAG_USBMIDI::setHandleNoteOn(NoteOnCallback fptr) { handleNoteOn = fptr; }
void CTAG_USBMIDI::setHandleNoteOff(NoteOffCallback fptr) { handleNoteOff = fptr; }
void CTAG_USBMIDI::setHandleControlChange(ControlChangeCallback fptr) { handleControlChange = fptr; }
void CTAG_USBMIDI::setHandleProgramChange(ProgramChangeCallback fptr) { handleProgramChange = fptr; }
void CTAG_USBMIDI::setHandlePitchBend(PitchBendCallback fptr) { handlePitchBend = fptr; }


// --- UTILITY HELPER FUNCTIONS ---
String CTAG_USBMIDI::getNoteName(byte noteNumber) {
  if (noteNumber > 127) return "Invalid";
  byte noteIndex = noteNumber % 12;
  int octave = (noteNumber / 12) - 1;
  String result = noteNames[noteIndex];
  result += String(octave);
  return result;
}


// --- PRIVATE PARSER ---
void CTAG_USBMIDI::parse(byte midiByte) {
  if (midiByte >= 0xF8) { return; } // Ignore Real-Time messages
  
  if (midiByte & 0x80) { // It's a new status byte
    status = midiByte;
    data1 = 0; data2 = 0;
  } else { // It's a data byte
    if (data1 == 0) {
      data1 = midiByte;
      byte command = status & 0xF0;
      if (command == 0xC0 || command == 0xD0) { // 2-byte message
        if (handleProgramChange && command == 0xC0) { handleProgramChange((status & 0x0F) + 1, data1); }
        status = 0; data1 = 0;
      }
    } else {
      data2 = midiByte;
      byte command = status & 0xF0;
      byte channel = (status & 0x0F) + 1;
      switch(command) {
        case 0x90: // Note On
          if (data2 > 0) { if (handleNoteOn) handleNoteOn(channel, data1, data2); }
          else { if (handleNoteOff) handleNoteOff(channel, data1, data2); }
          break;
        case 0x80: // Note Off
          if (handleNoteOff) handleNoteOff(channel, data1, data2);
          break;
        case 0xB0: // Control Change
          if (handleControlChange) handleControlChange(channel, data1, data2);
          break;
        case 0xE0: // Pitch Bend
          if (handlePitchBend) {
            int value = ((int)data2 << 7) | (int)data1;
            handlePitchBend(channel, value - 8192);
          }
          break;
      }
      data1 = 0; data2 = 0;
    }
  }
}