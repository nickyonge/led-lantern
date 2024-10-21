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

#define ENABLE_ANIMATION

#ifdef ENABLE_ANIMATION
#include <Random16.h>
#define ANIM_FPS 30 // Frames per second the animation will render at
#endif

void setupLEDs();
void loopLEDs();

// shift the current LED colour by the given amount (HSV hue, 0-255)
void shiftLEDColor(int delta);
// output the current colour information to FastLED
void updateLEDs();

// debug convenience function to shift LED colour by 128 (opposite end of the spectrum from current)
void jumpLEDColor();
// debug convenience function to set every LED colour to `CRGB::RED`
void testLEDColor();

#ifdef ENABLE_ANIMATION
// process one frame of LED animation
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