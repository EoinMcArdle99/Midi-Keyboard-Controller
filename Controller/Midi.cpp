#include "Midi.h"

Midi::Midi() {
  const byte MIDI_BAUD_RATE = 31250;
  Serial.begin(MIDI_BAUD_RATE);
  selectInstrument();
  setPitchBend();
}

void Midi::selectInstrument() {
  Serial.write(0xC0);
  Serial.write(0x00);
}

void Midi::setPitchBend() {
  Serial.write(0xE0);
  Serial.write(0x00);
  Serial.write(0x60);
}

void Midi::noteOn(u16 keyNumber, u32 interval) {
  const byte MIDI_NOTE_ON = 0x90;
  Serial.write(MIDI_NOTE_ON);
  Serial.write(keys[keyNumber].pitch);
  Serial.write(calculateVelocity(interval));
}

void Midi::noteOff(u16 keyNumber) {
  const byte MIDI_NOTE_OFF = 0x80;
  Serial.write(MIDI_NOTE_OFF);
  Serial.write(keys[keyNumber].pitch);
  Serial.write(0);
}

byte Midi::calculateVelocity(u32 interval) {
  const int result = 65 - (interval / 6);
  if ( result < 0) {
    return 0;
  }
  return result;
}



