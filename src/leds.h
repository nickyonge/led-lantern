#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <FastLED.h>

#include "pindef.h"

#define CHIPSET WS2812B
#define RGB_ORDER GRB

#define NUM_LEDS 12

#define LED_BRIGHTNESS 120

void setupLEDs();
void shiftLEDColor(int delta); // shift the current LED colouru by the given amount
void updateLEDs();

#endif