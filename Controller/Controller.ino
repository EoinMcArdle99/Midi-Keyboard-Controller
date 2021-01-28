/* Select Key Bank (4 notes) */
const int SELECT_A = A0;
const int SELECT_B  = 13;
const int SELECT_C  = 12;

/* Select chip. */
const int ENABLE_G2A  = 11;
const int ENABLE_G2B = 10;

/* Input from keys */
const int INPUT_0  = 9;
const int INPUT_1  = 8;
const int INPUT_2  = A1;
const int INPUT_3 = 7;
const int INPUT_4 = 5;
const int INPUT_5 = 4;
const int INPUT_6  = 3;
const int INPUT_7  = 2;

/* Keyboard info */
const int NUMBER_OF_DECODERS = 3;
const int NUMBER_OF_KEYS = 88;
const int NUMBER_OF_DECODER_OUTPUTS = 8;
const int NUMBER_OF_KEYS_PER_OUTPUT = 4;

enum key_state_t{
  KEY_UP,
  KEY_DOWN,
  KEY_GOING_DOWN,
  KEY_GOING_UP
};

struct key_data_t{
  byte pitch;
  unsigned long t1;
  enum key_state_t state;
} keys[NUMBER_OF_KEYS];

void setup(){
    pinMode(SELECT_A, OUTPUT);
    pinMode(SELECT_B, OUTPUT);
    pinMode(SELECT_C, OUTPUT);

    pinMode(ENABLE_G2A, OUTPUT);
    pinMode(ENABLE_G2B, OUTPUT);

    pinMode(INPUT_0, INPUT_PULLUP);
    pinMode(INPUT_1, INPUT_PULLUP);
    pinMode(INPUT_2, INPUT_PULLUP);
    pinMode(INPUT_3, INPUT_PULLUP);
    pinMode(INPUT_4, INPUT_PULLUP);
    pinMode(INPUT_5, INPUT_PULLUP);
    pinMode(INPUT_6, INPUT_PULLUP);
    pinMode(INPUT_7, INPUT_PULLUP);

    keySetup();
    //Serial.begin(31250);
    Serial.begin(9600);
}

void loop(){
  /* Current chip and bank selection and number of banks in selected decoder */
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
      /* Select bank of keys for decoder */
      digitalWrite(SELECT_A, bank & 1);
      digitalWrite(SELECT_B, (bank & 2) >> 1);
      digitalWrite(SELECT_C, (bank & 4) >> 2);
      
      /* Handle key presses */
      unsigned short keyNumber = (bank * 4) + (chip * 32);
      (digitalRead(INPUT_0) == 0) ? switchOneClosed(keyNumber) : switchOneOpen(keyNumber);
      (digitalRead(INPUT_1) == 0) ? switchTwoClosed(keyNumber) : switchTwoOpen(keyNumber);

      keyNumber++;
      (digitalRead(INPUT_2) == 0) ? switchOneClosed(keyNumber) : switchOneOpen(keyNumber);
      (digitalRead(INPUT_3) == 0) ? switchTwoClosed(keyNumber) : switchTwoOpen(keyNumber);

      keyNumber++;
      (digitalRead(INPUT_4) == 0) ? switchOneClosed(keyNumber) : switchOneOpen(keyNumber);
      (digitalRead(INPUT_5) == 0) ? switchTwoClosed(keyNumber) : switchTwoOpen(keyNumber);

      keyNumber++;
      (digitalRead(INPUT_6) == 0) ? switchOneClosed(keyNumber) : switchOneOpen(keyNumber);
      (digitalRead(INPUT_7) == 0) ? switchTwoClosed(keyNumber) : switchTwoOpen(keyNumber);
    }
  }
}

void keySetup(){
    for(int i = 0; i < 88; i++){
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
  Serial.println(interval);
  return 0x10;
}

void noteOn(unsigned short keyNumber){
    // Send status byte;
    byte status = 0x90;
    byte velocity = 0x32;
//    Serial.write(status);
//    Serial.write(keys[keyNumber].pitch);
//    Serial.write(velocity);
    //Serial.println(keyNumber);
}

void noteOff(unsigned short keyNumber){
    //Serial.println(keyNumber);
    byte status = 0x80;
//    Serial.write(status);
//    Serial.write(keys[keyNumber].pitch);
//    Serial.write(0);
}
