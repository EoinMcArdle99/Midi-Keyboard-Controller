#ifndef _CONTROLLER_H
#define _CONTROLLER_H

/* Keyboard info */
const int NUMBER_OF_DECODERS = 3;
const int NUMBER_OF_KEYS = 88;
const int NUMBER_OF_DECODER_OUTPUTS = 8;
const int NUMBER_OF_KEYS_PER_OUTPUT = 4;

/* Midi info */
const byte NOTE_ON = 0x90;
const byte NOTE_OFF = 0x80;
const byte VELOCITY = 60;
const unsigned short BAUD_RATE = 31250;

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
