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

// shift the current LED colour by the given amount (HSV hue, 0-255)
void shiftLEDColor(int delta);
// convenience function to shift LED colour by 128 (opposite end of the spectrum from current)
void jumpLEDColor();
// output the current colour information to FastLED
void updateLEDs();

#ifndef DISABLE_ANIMATION
// LED animation sourced from FastLED's Fire2012
void animateLEDs();
#endif

// call from sleep.h when device is put to sleep (to disable LED display)
void sleepLEDs();
// call from sleep.h when device wakes up (to re-enable LED display)
void wakeLEDs();

// clear current LED display and data (sets all LEDs to black, clears buffer)
void clearLEDLocalData();
// load LED colour data from EEPROM
void loadLEDData();
// save LED colour data to EEPROM
void saveLEDData();

#endif // LEDS_H