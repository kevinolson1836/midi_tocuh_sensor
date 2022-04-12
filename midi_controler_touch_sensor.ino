/*
  Made by Gustavo Silveira, 2019.
  - This Sketch reads the Arduino's digital and analog ports and send midi notes and midi control change

  http://www.musiconerd.com
  http://www.youtube.com/musiconerd
  http://facebook.com/musiconerdmusiconerd
  http://instagram.com/musiconerd/
  http://www.gustavosilveira.net
  gustavosilveira@musiconerd.com

  If you are using for anything that's not for personal use don't forget to give credit.

  PS: Just change the value that has a comment like " //* "

*/

/////////////////////////////////////////////
// Choosing your board
// Define your board, choose:
// "ATMEGA328" if using ATmega328 - Uno, Mega, Nano...
// "ATMEGA32U4" if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
// "TEENSY" if using a Teensy board
// "DEBUG" if you just want to debug the code in the serial monitor
// you don't need to comment or uncomment any MIDI library below after you define your board

#define ATMEGA328 1 //* put here the uC you are using, like in the lines above followed by "1", like "ATMEGA328 1", "DEBUG 1", etc.



/////////////////////////////////////////////
// mpr121(touch sensor) stuff here
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

Adafruit_MPR121 cap1 = Adafruit_MPR121();
uint16_t lasttouched1 = 0;
uint16_t currtouched1 = 0;

//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
Adafruit_MPR121 cap2 = Adafruit_MPR121();
uint16_t lasttouched2 = 0;
uint16_t currtouched2 = 0;

Adafruit_MPR121 cap3 = Adafruit_MPR121();
uint16_t lasttouched3 = 0;
uint16_t currtouched3 = 0;

Adafruit_MPR121 cap4 = Adafruit_MPR121();
uint16_t lasttouched4 = 0;
uint16_t currtouched4 = 0;
//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
/////////////////////////////////////////////
// LIBRARIES
// -- Defines the MIDI library -- //

// if using with ATmega328 - Uno, Mega, Nano...
#ifdef ATMEGA328
#include <MIDI.h> // by Francois Best
MIDI_CREATE_DEFAULT_INSTANCE();

// if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
#elif ATMEGA32U4
#include "MIDIUSB.h"

#endif
// ---- //


/////////////////////////////////////////////
// notes
const int N_NOTES = 12; //*  total numbers of buttons
const int note1[N_NOTES] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21}; //* pins of each button connected straight to the Arduino
const int note2[N_NOTES] = {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}; //* pins of each button connected straight to the Arduino
const int note3[N_NOTES] = {70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81}; //* pins of each button connected straight to the Arduino
const int note4[N_NOTES] = {90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101}; //* pins of each button connected straight to the Arduino



/////////////////////////////////////////////




/////////////////////////////////////////////
// BUTTONS
const int N_BUTTONS = 0; //*  total numbers of buttons
const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {}; //* pins of each button connected straight to the Arduino

int buttonCState[N_BUTTONS] = {};        // stores the button current value
int buttonPState[N_BUTTONS] = {};        // stores the button previous value

//#define pin13 1 //* uncomment if you are using pin 13 (pin with led), or comment the line if not using
byte pin13index = 12; //* put the index of the pin 13 of the buttonPin[] array if you are using, if not, comment

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {};  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    //* the debounce time; increase if the output flickers

/////////////////////////////////////////////
// POTENTIOMETERS
const int N_POTS = 0; //* total numbers of pots (slide & rotary)
const int POT_ARDUINO_PIN[N_POTS] = {}; //* pins of each pot connected straight to the Arduino

int potCState[N_POTS] = {}; // Current state of the pot
int potPState[N_POTS] = {}; // Previous state of the pot
int potVar = 0; // Difference between the current and previous state of the pot

int midiCState[N_POTS] = {}; // Current state of the midi value
int midiPState[N_POTS] = {}; // Previous state of the midi value

const int TIMEOUT = 300; //* Amount of time the potentiometer will be read after it exceeds the varThreshold
const int varThreshold = 10; //* Threshold for the potentiometer signal variation
boolean potMoving = true; // If the potentiometer is moving
unsigned long PTime[N_POTS] = {}; // Previously stored time
unsigned long timer[N_POTS] = {}; // Stores the time that has elapsed since the timer was reset

/////////////////////////////////////////////
// MIDI
byte midiCh = 1; //* MIDI channel to be used
byte note = 10; //* Lowest note to be used hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
byte cc = 1; //* Lowest MIDI CC to be used

/////////////////////////////////////////////
// SETUP
void setup() {

  // Baud Rate
  // use if using with ATmega328 (uno, mega, nano...)
  // 31250 for MIDI class compliant | 115200 for Hairless MIDI
  Serial.begin(115200); //*

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
#ifdef DEBUG
Serial.println("Debug mode");
Serial.println();
#endif

  // Buttons
  // Initialize buttons with pull up resistors
  for (int i = 0; i < N_BUTTONS; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

#ifdef pin13 // initialize pin 13 as an input
pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
      Serial.println("???????");

  if (!cap1.begin(0x5A)) {
    Serial.println("MPR121 1 not found, check wiring?");
    while (1);
   }
      Serial.println("MPR121 board 1 found !");

//   hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
if (!cap2.begin(0x5B)) {
    Serial.println("MPR121 2 not found, check wiring?");
    while (1);
   }
      Serial.println("MPR121 board 2 found!");
   
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
if (!cap3.begin(0x5C)) {
    Serial.println("MPR121 3 not found, check wiring?");
    while (1);
   }
      Serial.println("MPR121 board 3 found!");
   

     // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
if (!cap4.begin(0x5D)) {
    Serial.println("MPR121 4 not found, check wiring?");
    while (1);
   }
      Serial.println("MPR121 board 4 found!");
}
//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
/////////////////////////////////////////////
// LOOP
void loop() {
//
//  buttons();
//  potentiometers();

  currtouched1 = cap1.touched();
  currtouched2 = cap2.touched();
  currtouched3 = cap3.touched();
  currtouched4 = cap4.touched();
//   
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched1 & _BV(i)) && !(lasttouched1 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" touched on board 1");
        MIDI.sendNoteOn(note1[i], 127, midiCh); // note, velocity, channel

    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched1 & _BV(i)) && (lasttouched1 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" released on board 1");
        MIDI.sendNoteOn(note1[i], 0, midiCh); // note, velocity, channel

    }
  }
    lasttouched1 = currtouched1;

//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched2 & _BV(i)) && !(lasttouched2 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" touched on board 2");
        MIDI.sendNoteOn(note2[i], 127, midiCh); // note, velocity, channel

    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched2 & _BV(i)) && (lasttouched2 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" released on board 2");
        MIDI.sendNoteOn(note2[i], 0, midiCh); // note, velocity, channel
    }
  }

    for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched3 & _BV(i)) && !(lasttouched3 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" touched on board 3");
        MIDI.sendNoteOn(note3[i], 127, midiCh); // note, velocity, channel

    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched3 & _BV(i)) && (lasttouched3 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" released on board 3");
        MIDI.sendNoteOn(note3[i], 0, midiCh); // note, velocity, channel

    }
  }

    for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched4 & _BV(i)) && !(lasttouched4 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" touched on board 4");
        MIDI.sendNoteOn(note4[i], 127, midiCh); // note, velocity, channel

    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched4 & _BV(i)) && (lasttouched4 & _BV(i)) ) {
//      Serial.print(i); Serial.println(" released on board 4");
        MIDI.sendNoteOn(note4[i], 0, midiCh); // note, velocity, channel

    }
  }
  
  lasttouched1 = currtouched1;
  lasttouched2 = currtouched2;
  lasttouched3 = currtouched3;
  lasttouched4 = currtouched4;
}
//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh


/////////////////////////////////////////////
// BUTTONS
void buttons() {

  for (int i = 0; i < N_BUTTONS; i++) {

    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);  // read pins from arduino

#ifdef pin13
if (i == pin13index) {
buttonCState[i] = !buttonCState[i]; // inverts the pin 13 because it has a pull down resistor instead of a pull up
}
#endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {

          // Sends the MIDI note ON accordingly to the chosen board
#ifdef ATMEGA328
// use if using with ATmega328 (uno, mega, nano...)
MIDI.sendNoteOn(note + i, 127, midiCh); // note, velocity, channel

#elif ATMEGA32U4
// use if using with ATmega32U4 (micro, pro micro, leonardo...)
noteOn(midiCh, note + i, 127);  // channel, note, velocity
MidiUSB.flush();

#elif TEENSY
//do usbMIDI.sendNoteOn if using with Teensy
usbMIDI.sendNoteOn(note + i, 127, midiCh); // note, velocity, channel

#elif DEBUG
Serial.print(i);
Serial.println(": button on");
#endif

        }
        else {

          // Sends the MIDI note OFF accordingly to the chosen board
#ifdef ATMEGA328
// use if using with ATmega328 (uno, mega, nano...)
MIDI.sendNoteOn(note + i, 0, midiCh); // note, velocity, channel

#elif ATMEGA32U4
// use if using with ATmega32U4 (micro, pro micro, leonardo...)
noteOn(midiCh, note + i, 0);  // channel, note, velocity
MidiUSB.flush();

#elif TEENSY
//do usbMIDI.sendNoteOn if using with Teensy
usbMIDI.sendNoteOn(note + i, 0, midiCh); // note, velocity, channel

#elif DEBUG
Serial.print(i);
Serial.println(": button off");
#endif

        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

/////////////////////////////////////////////
// POTENTIOMETERS
void potentiometers() {


  for (int i = 0; i < N_POTS; i++) { // Loops through all the potentiometers

    potCState[i] = analogRead(POT_ARDUINO_PIN[i]); // reads the pins from arduino

    midiCState[i] = map(potCState[i], 0, 1023, 0, 127); // Maps the reading of the potCState to a value usable in midi

    potVar = abs(potCState[i] - potPState[i]); // Calculates the absolute value between the difference between the current and previous state of the pot

    if (potVar > varThreshold) { // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis(); // Stores the previous time
    }

    timer[i] = millis() - PTime[i]; // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) { // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
      potMoving = true;
    }
    else {
      potMoving = false;
    }

    if (potMoving == true) { // If the potentiometer is still moving, send the change control
      if (midiPState[i] != midiCState[i]) {

        // Sends the MIDI CC accordingly to the chosen board
#ifdef ATMEGA328
// use if using with ATmega328 (uno, mega, nano...)
MIDI.sendControlChange(cc + i, midiCState[i], midiCh); // cc number, cc value, midi channel

#elif ATMEGA32U4
//use if using with ATmega32U4 (micro, pro micro, leonardo...)
controlChange(midiCh, cc + i, midiCState[i]); //  (channel, CC number,  CC value)
MidiUSB.flush();

#elif TEENSY
//do usbMIDI.sendControlChange if using with Teensy
usbMIDI.sendControlChange(cc + i, midiCState[i], midiCh); // cc number, cc value, midi channel

#elif DEBUG
Serial.print("Pot: ");
Serial.print(i);
Serial.print(" ");
Serial.println(midiCState[i]);
//Serial.print("  ");
#endif

        potPState[i] = potCState[i]; // Stores the current reading of the potentiometer to compare with the next
        midiPState[i] = midiCState[i];
      }
    }
  }
}

/////////////////////////////////////////////
// if using with ATmega32U4 (micro, pro micro, leonardo...)
#ifdef ATMEGA32U4

// Arduino (pro)micro midi functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
#endif
