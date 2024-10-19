#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

#include <RotaryEncoder.h>

#include "pindef.h"
#include "leds.h"

void setupInput();
void loopInput();

void onInterrupt(); // callback for pin interrupt

static int encPos = 0;         // current position of rotary encoder
static bool encSwitch = false; // is rotary encoder switch currently pressed?
volatile bool lastInterrupted; // was interrupt called before last loop cylce?

#endif