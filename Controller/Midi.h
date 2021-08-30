#ifndef _MIDI_H
#define _MIDI_H

#include "Arduino.h"

public static class Midi
{
private:
  byte calculateVelocity(u32 interval);
  void selectInstrument();
  void setPitchBend();
public:
  Midi();
  void noteOn(u16 keyNumber, u32 interval);
  void noteOff(u16 keyNumber);
};

#endif