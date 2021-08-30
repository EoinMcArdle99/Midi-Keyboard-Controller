/*
  A full 88 key keyboard controller. Scans keyboard for key presses 
  and outputs key presses in midi format.
*/

#include "Controller.h"

Midi midi();

void setup(){
  pinMode(SELECT_A, OUTPUT);
  pinMode(SELECT_B, OUTPUT);
  pinMode(SELECT_C, OUTPUT);

  pinMode(ENABLE_G2A, OUTPUT);
  pinMode(ENABLE_G2B, OUTPUT);

  setPullUpInputPins();
  setKeyPitchesAndCurrentState();
}

void setPullUpInputPins() {
  for(int i = 0; i < NUMBER_OF_DECODER_OUTPUTS; i++){
    pinMode(INPUTS[i], INPUT_PULLUP);
  }
}

void setKeyPitchesAndCurrentState(){
  for(int i = 0; i < NUMBER_OF_KEYS; i++){
    keys[i].pitch = i + 21;
    keys[i].state = KEY_UP;
  }
}

void loop(){
  unsigned short decoderIndex, selectedBank,keyNumber;
  for (decoderIndex = 0, keyNumber = 0; decoderIndex < NUMBER_OF_DECODERS; decoderIndex++) {
    selectDecoder(decoderIndex);
    const u16 numBanksInDecoder = getNumBanksInDecoder(decoderIndex);
    for (selectedBank = 0; selectedBank < numBanksInDecoder; selectedBank++) {
      selectKeyBank(selectedBank);
      for (u16 i = 0; i < NUMBER_OF_DECODER_OUTPUTS; i += 2, keyNumber++) {
        readSwitches(i, keyNumber);
      }
    }
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

unsigned short getNumBanksInDecoder(unsigned short decoderIndex) {
  /* Last decoderIndex onboard only uses first 6 outputs. */
  return (decoderIndex == 2) ? NUMBER_OF_DECODER_OUTPUTS - 2 : NUMBER_OF_DECODER_OUTPUTS;
}

void selectKeyBank(u16 selectedBank) {
  digitalWrite(SELECT_A, calculateSelect_aState(selectedBank));
  digitalWrite(SELECT_B, calculateSelect_bState(selectedBank));
  digitalWrite(SELECT_C, calculateSelect_cState(selectedBank));
}

u16 calculateSelect_aState(u16 selectedBank) {
  return selectedBank & 1;
}

u16 calculateSelect_bState(u16 selectedBank) {
  return (selectedBank & 2) >> 1;
}

u16 calculateSelect_cState(u16 selectedBank) {
  return (selectedBank & 4) >> 2;
}

void readSwitches(u16 switchIndex, u16 keyNumber) {
  (digitalRead(INPUTS[switchIndex]) == LOW) ? switchOneClosed(keyNumber) : switchOneOpen(keyNumber);
  (digitalRead(INPUTS[switchIndex + 1]) == LOW) ? switchTwoClosed(keyNumber) : switchTwoOpen(keyNumber);
}

void switchOneClosed(u16 keyNumber) {
  if (keys[keyNumber].state == KEY_UP) {
    keys[keyNumber].t1 = millis();
    keys[keyNumber].state = KEY_GOING_DOWN;
  }
}

void switchOneOpen(u16 keyNumber) {
  keys[keyNumber].state = KEY_UP;
}

void switchTwoClosed(u16 keyNumber) {
  if (keys[keyNumber].state == KEY_GOING_DOWN) {
    keys[keyNumber].state = KEY_DOWN;
    unsigned long interval = millis() - keys[keyNumber].t1;
    midi.noteOn(keyNumber, interval);
  }
} 

void switchTwoOpen(u16 keyNumber) {
  if (keys[keyNumber].state == KEY_DOWN) {
    keys[keyNumber].state = KEY_GOING_UP;
    midi.noteOff(keyNumber);
  }
}
