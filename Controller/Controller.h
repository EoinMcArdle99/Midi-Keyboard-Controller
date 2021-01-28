#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#define DEV

/* Keyboard info */
const int NUMBER_OF_DECODERS = 3;
const int NUMBER_OF_KEYS = 88;
const int NUMBER_OF_DECODER_OUTPUTS = 8;
const int NUMBER_OF_KEYS_PER_OUTPUT = 4;

/* Select Key Bank (4 notes) */
const int SELECT_A = A0;
const int SELECT_B  = 13;
const int SELECT_C  = 12;

/* Select chip. */
const int ENABLE_G2A  = 11;
const int ENABLE_G2B = 10;

/* Keyboard inputs. */
const int INPUTS[] = {9, 8, A1, 7, 5, 4, 3, 2};

/* Possible states a key can be in. */
enum KeyState{
  KEY_UP,
  KEY_DOWN,
  KEY_GOING_DOWN,
  KEY_GOING_UP
};

/* Represents each key on board. */
struct KeyData{
  byte pitch;
  unsigned long t1;
  enum KeyState state;
} keys[NUMBER_OF_KEYS];

#ifdef DEV
#define BREAK "-------------------------------"
int numVels = 0;
unsigned long vels[50];
#endif

#endif
