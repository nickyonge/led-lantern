#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <FastLED.h>

#include "pindef.h"
#include "savedata.h"

#define CHIPSET WS2812B
#define RGB_ORDER GRB

#define NUM_LEDS 12

#define LED_BRIGHTNESS 120

void setupLEDs();
void shiftLEDColor(int delta); // shift the current LED colour by the given amount
void updateLEDs();
void clearLEDLocalData();
void loadLEDData();
void saveLEDData();

#endif // LEDS_H