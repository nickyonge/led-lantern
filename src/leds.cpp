#include "leds.h"

CRGB leds[NUM_LEDS];

void setupLEDs()
{
    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.addLeds<CHIPSET, LED_DATA_PIN, RGB_ORDER>(leds, NUM_LEDS);
    updateLEDs();
}

void updateLEDs()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB::Magenta;
    }
    FastLED.show();
}