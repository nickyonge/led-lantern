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

#define LED_MAX_BRIGHTNESS 120 // max brightness permitted by FastLED
#define LED_MIN_BRIGHTNESS 10  // min brightness given via HSV values

#define DEBUG_FLASH_LED_0 // if defined, flash LED 0 red on/off every second

// #define LED_MAX_MILLIAMP_DRAW 250 // if defined, set max mA/H draw permitted by FastLED

// #define ENABLE_ANIMATION // allow animation rendering?

#ifdef ENABLE_ANIMATION
#include <Random16.h>
#define ANIM_FPS 30 // Frames per second the animation will render at
#endif

void setupLEDs();
void loopLEDs();

// shift the current LED colour by the given amount (HSV hue, 0 - 255, wrapping)
void shiftLEDColor(int delta);
// shift the current LED brightness by the given amount (HSV value, LED_MIN_BRIGHTNESS - 255, clamped)
void shiftLEDBrightness(int delta);
// output the current colour information to FastLED
void updateLEDs();

// debug convenience function to shift LED colour by 128 (opposite end of the spectrum from current)
void jumpLEDColor();
// debug convenience function to set every LED colour to `CRGB::RED`
void testLEDColor();

// return a byte for the LED's current HSV brightness value (V)
byte getLEDBrightness();

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