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
  #ifdef DEV
    Serial.begin(9600);
  #else
    Serial.begin(31250);
  #endif 
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
    keys[keyNumber].t1 = micros();
    keys[keyNumber].state = KEY_GOING_DOWN;
  }
}

void switchOneOpen(unsigned short keyNumber){
  keys[keyNumber].state = KEY_UP;
}

void switchTwoClosed(unsigned short keyNumber){
  if(keys[keyNumber].state == KEY_GOING_DOWN){
    keys[keyNumber].state = KEY_DOWN;
    unsigned long interval = micros() - keys[keyNumber].t1;
    byte velocity = calculateVelocity(interval);
    noteOn(keyNumber);
  }
} 

void switchTwoOpen(unsigned short keyNumber){
  if(keys[keyNumber].state == KEY_DOWN){
    keys[keyNumber].state = KEY_GOING_UP;
        noteOff(keyNumber);
  }
}

byte calculateVelocity(unsigned long interval){
  #ifdef DEV
    calcAverageVelocity(interval);
  #endif
  return 0x10;
}

void noteOn(unsigned short keyNumber){
    byte stat = 0x90;
    byte velocity = 0x32;

    #ifndef DEV
      Serial.write(stat);
      Serial.write(keys[keyNumber].pitch);
      Serial.write(velocity);
    #endif
}

void noteOff(unsigned short keyNumber){
    byte stat = 0x80;

    #ifndef DEV
      Serial.write(stat);
      Serial.write(keys[keyNumber].pitch);
      Serial.write(0);
    #endif
}

#ifdef DEV
void calcAverageVelocity(unsigned long interval){
  vels[numVels] = interval;
  numVels++;
  unsigned long average, maxVel, minVel;
  average = 0;
  maxVel = vels[0];
  minVel = vels[0];
  for(int i = 0; i < numVels; i++){
    average += vels[i];
    if(maxVel < vels[i]){
      maxVel = vels[i];
    }
    if(minVel > vels[i]){
      minVel = vels[i];
    }
  }
  average /= numVels;
  Serial.println(BREAK);
  Serial.print("Interval: ");
  Serial.println(interval);
  Serial.print("Average: ");
  Serial.println(average);
  Serial.print("Max: ");
  Serial.println(maxVel);
  Serial.print("Min: ");
  Serial.println(minVel);
  Serial.println(BREAK);
}
#endif
