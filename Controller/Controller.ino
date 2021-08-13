/*
  A full 88 key keyboard controller. Scans keyboard for key presses 
  and outputs key presses in midi format.
*/

#include "Controller.h"

void setup(){

  /* Init decoder select pins. */
  pinMode(SELECT_A, OUTPUT);
  pinMode(SELECT_B, OUTPUT);
  pinMode(SELECT_C, OUTPUT);

  /* Init decoder enable pins. */
  pinMode(ENABLE_G2A, OUTPUT);
  pinMode(ENABLE_G2B, OUTPUT);

  /* Init input pins. */
  for(int i = 0; i < 8; i++){
    pinMode(INPUTS[i], INPUT_PULLUP);
  }

  /* Set default key state. */
  keySetup();

  /* Begin serial comms for midi output. */
  Serial.begin(BAUD_RATE);

  selectInstrument();
  setPitchBend();
}

/* Poll for key presses. */
void loop(){

  /* Current chip and bank selection and number of banks in selected decoder. */
  unsigned short chip, bank, numBanks;

  /* Cycle through decoders */
  for(chip = 0; chip < NUMBER_OF_DECODERS; chip++){
    /* Select decoder */
    digitalWrite(ENABLE_G2A, chip & 1);
    digitalWrite(ENABLE_G2B, (chip & 2) >> 1);
    
    /* Last chip onboard only uses first 6 outputs. */
    numBanks = (chip == 2) ? NUMBER_OF_DECODER_OUTPUTS - 2 : NUMBER_OF_DECODER_OUTPUTS;

    /* Cycle through key banks in decoder. */
    for(bank = 0; bank < numBanks; bank++){
      /* Select bank of keys for decoder. */
      digitalWrite(SELECT_A, bank & 1);
      digitalWrite(SELECT_B, (bank & 2) >> 1);
      digitalWrite(SELECT_C, (bank & 4) >> 2);
      
      /* Handle key presses. */
      unsigned short keyNumber = (bank * NUMBER_OF_KEYS_PER_OUTPUT) + (chip * NUMBER_OF_KEYS_PER_OUTPUT * NUMBER_OF_DECODER_OUTPUTS);
      for(int i = 0; i < NUMBER_OF_DECODER_OUTPUTS; i += 2, keyNumber++){
        (digitalRead(INPUTS[i]) == 0) ? switchOneClosed(keyNumber) : switchOneOpen(keyNumber);
        (digitalRead(INPUTS[i + 1]) == 0) ? switchTwoClosed(keyNumber) : switchTwoOpen(keyNumber);
      }
    }
  }
}

void keySetup(){
  /* Init each key with pitch and default state. */
  for(int i = 0; i < NUMBER_OF_KEYS; i++){
      keys[i].pitch = i + 21;
      keys[i].state = KEY_UP;
  }
}

void switchOneClosed(unsigned short keyNumber){
  if(keys[keyNumber].state == KEY_UP){
    keys[keyNumber].t1 = millis();
    keys[keyNumber].state = KEY_GOING_DOWN;
  }
}

void switchOneOpen(unsigned short keyNumber){
  keys[keyNumber].state = KEY_UP;
}

void switchTwoClosed(unsigned short keyNumber){
  if(keys[keyNumber].state == KEY_GOING_DOWN){
    keys[keyNumber].state = KEY_DOWN;
    unsigned long interval = millis() - keys[keyNumber].t1;
    noteOn(keyNumber, interval);
  }
} 

void switchTwoOpen(unsigned short keyNumber){
  if(keys[keyNumber].state == KEY_DOWN){
    keys[keyNumber].state = KEY_GOING_UP;
      noteOff(keyNumber);
  }
}

void noteOn(unsigned short keyNumber, unsigned long interval){
    byte vel = 30;
    Serial.write(NOTE_ON);
    Serial.write(keys[keyNumber].pitch);
    Serial.write(calculateVelocity(interval));
}

void noteOff(unsigned short keyNumber){
    Serial.write(NOTE_OFF);
    Serial.write(keys[keyNumber].pitch);
    Serial.write(0);
}

void selectInstrument(){
  Serial.write(0xC0);
  Serial.write(0x00);
}

void setPitchBend(){
  Serial.write(0xE0);
  Serial.write(0x00);
  Serial.write(0x60);
}

byte calculateVelocity(unsigned long interval){
  const int result = 65 - (interval / 6);
  if( result < 0) {
    return 0;
  }
  return result;
}

int curve(unsigned long interval) {
  if (interval > 400) {
    return 0;
  }
  int num1 = (int) interval - 400;
  int num = pow(num1, 2);

  //const int result = (pow((interval - 300), 2) / 1000);
  return num / 1200;
}
