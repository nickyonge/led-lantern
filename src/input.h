#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

#include <RotaryEncoder.h>

#include "pindef.h"
#include "leds.h"

void setupInput();
void loopInput();

void onInterrupt(); // callback for pin interrupt

#endif // INPUT_H