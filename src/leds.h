#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <FastLED.h>

#include "main.h"
#include "pindef.h"
#include "savedata.h"

#define CHIPSET WS2812B
#define RGB_ORDER GRB

#define NUM_LEDS 12
#define LED_BRIGHTNESS 120

#define DISABLE_ANIMATION

#ifndef DISABLE_ANIMATION
#define ANIM_FPS 30
constexpr int animInterval = (1000 / ANIM_FPS) / DELAY_INTERVAL;
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING 55
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120
#endif

void setupLEDs();
void loopLEDs();

void shiftLEDColor(int delta); // shift the current LED colour by the given amount
void updateLEDs();
#ifndef DISABLE_ANIMATION
// LED animation sourced from FastLED's Fire2012
void animateLEDs();
#endif

void clearLEDLocalData();
void loadLEDData();
void saveLEDData();

#endif // LEDS_H