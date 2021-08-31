/*
  A 88 piano keyboard controller for use with Ketron Klavipro sound module. 
  Scans keyboard for key presses and outputs key presses in midi format.
*/

#include "Controller.h"

void setup() {
  pinMode(SELECT_A, OUTPUT);
  pinMode(SELECT_B, OUTPUT);
  pinMode(SELECT_C, OUTPUT);

  pinMode(ENABLE_G2A, OUTPUT);
  pinMode(ENABLE_G2B, OUTPUT);

  pinMode(SUSTAIN, INPUT_PULLUP);

  setPullUpInputPins();
  setKeyPitchesAndCurrentState();

  initMidi();
}

void initMidi() {
  const int MIDI_BAUD_RATE = 31250;
  Serial.begin(MIDI_BAUD_RATE);
  selectInstrument();
  setPitchBend();
}

void selectInstrument() {
  const byte SELECT_INSTRUMENT_COMMAND = 0xC0;
  const byte STEREO_GRAND_PIANO = 0x00;
  Serial.write(SELECT_INSTRUMENT_COMMAND);
  Serial.write(STEREO_GRAND_PIANO);
}

void setPitchBend() {
  const byte PITCH_BEND_COMMAND = 0xE0;
  Serial.write(PITCH_BEND_COMMAND);
  Serial.write(0x00);
  Serial.write(0x60);
}

void setPullUpInputPins() {
  for (int i = 0; i < NUMBER_OF_DECODER_OUTPUTS; i++) {
    pinMode(INPUTS[i], INPUT_PULLUP);
  }
}

void setKeyPitchesAndCurrentState() {
  for (int i = 0; i < NUMBER_OF_KEYS; i++) {
    const u16 PITCH_OFFSET = 21;
    keys[i].pitch = i + PITCH_OFFSET;
    keys[i].state = KEY_UP;
  }
}

void loop() {
  u16 decoderIndex, selectedBank, keyNumber;
  handleSustain();
  for (decoderIndex = 0, keyNumber = 0; decoderIndex < NUMBER_OF_DECODERS; decoderIndex++) {
    selectDecoder(decoderIndex);
    const u16 numBanksInDecoder = getNumBanksInDecoder(decoderIndex);
    for (selectedBank = 0; selectedBank < numBanksInDecoder; selectedBank++) {
      selectKeyBank(selectedBank);
      /* Increment i by 2 since each key has two switches */
      for (u16 i = 0; i < NUMBER_OF_DECODER_OUTPUTS; i += 2, keyNumber++) {
        readKey(i, keyNumber);
      }
    }
  }
}

void handleSustain() {
  const byte CTRL = 0xB0;
  const byte SUSTAIN_CTRL = 0x40;
  if((digitalRead(SUSTAIN) == LOW) && !sustainOn){
    Serial.write(CTRL);
    Serial.write(SUSTAIN_CTRL);
    Serial.write(64);
    sustainOn = true;
  }
  else if(digitalRead(SUSTAIN) != LOW && sustainOn){
    Serial.write(CTRL);
    Serial.write(SUSTAIN_CTRL);
    Serial.write(0);
    sustainOn = false;
  }
}

void selectDecoder(u16 decoderIndex) {
  digitalWrite(ENABLE_G2A, calculateEnableG2aPinState(decoderIndex));
  digitalWrite(ENABLE_G2B, calculateEnableG2bPinState(decoderIndex));
}

u16 calculateEnableG2aPinState(u16 decoderIndex) {
  return decoderIndex & 1;
}

u16 calculateEnableG2bPinState(u16 decoderIndex) {
  return (decoderIndex & 2) >> 1;
}

u16 getNumBanksInDecoder(u16 decoderIndex) {
  /* Last decoderIndex onboard only uses first 6 outputs. */
  return (decoderIndex == 2) ? NUMBER_OF_DECODER_OUTPUTS - 2 : NUMBER_OF_DECODER_OUTPUTS;
}

void selectKeyBank(u16 selectedBank) {
  digitalWrite(SELECT_A, calculateSelect_APinState(selectedBank));
  digitalWrite(SELECT_B, calculateSelect_BPinState(selectedBank));
  digitalWrite(SELECT_C, calculateSelect_CPinState(selectedBank));
}

u16 calculateSelect_APinState(u16 selectedBank) {
  return selectedBank & 1;
}

u16 calculateSelect_BPinState(u16 selectedBank) {
  return (selectedBank & 2) >> 1;
}

u16 calculateSelect_CPinState(u16 selectedBank) {
  return (selectedBank & 4) >> 2;
}

void readKey(u16 switchIndex, u16 keyNumber) {
  (digitalRead(INPUTS[switchIndex]) == LOW) ? switchOneClosed(keyNumber) : switchOneOpen(keyNumber);
  (digitalRead(INPUTS[switchIndex + 1]) == LOW) ? switchTwoClosed(keyNumber) : switchTwoOpen(keyNumber);
}

void switchOneClosed(u16 keyNumber) {
  if (keys[keyNumber].state == KEY_UP) {
    keys[keyNumber].timePressed = millis();
    keys[keyNumber].state = KEY_GOING_DOWN;
  }
}

void switchOneOpen(u16 keyNumber) {
  keys[keyNumber].state = KEY_UP;
}

void switchTwoClosed(u16 keyNumber) {
  if (keys[keyNumber].state == KEY_GOING_DOWN) {
    keys[keyNumber].state = KEY_DOWN;
    u32 interval = millis() - keys[keyNumber].timePressed;
    noteOn(keyNumber, interval);
  }
} 

void noteOn(u16 keyNumber, u32 interval) {
  const byte MIDI_NOTE_ON = 0x90;
  Serial.write(MIDI_NOTE_ON);
  Serial.write(keys[keyNumber].pitch);
  Serial.write(calculateVelocity(interval));
}

void switchTwoOpen(u16 keyNumber) {
  if (keys[keyNumber].state == KEY_DOWN) {
    keys[keyNumber].state = KEY_GOING_UP;
    noteOff(keyNumber);
  }
}

void noteOff(u16 keyNumber) {
  const byte MIDI_NOTE_OFF = 0x80;
  Serial.write(MIDI_NOTE_OFF);
  Serial.write(keys[keyNumber].pitch);
  Serial.write(0);
}

byte calculateVelocity(u32 interval) {
  /* Linear velocity function */
  const int result = 65 - (interval / 6);
  if ( result < 0) {
    return 0;
  }
  return result;
}
