#include "leds.h"

CRGB leds[NUM_LEDS];

static int ledColor = 213;

void setupLEDs()
{
    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.addLeds<CHIPSET, LED_DATA_PIN, RGB_ORDER>(leds, NUM_LEDS);
    updateLEDs();
}

void shiftLEDColor(int delta) {
    ledColor += delta;
    while (ledColor > 255) {
        ledColor -= 256;
    }
    while (ledColor < 0) {
        ledColor += 256;
    }
    updateLEDs();
}

void updateLEDs()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(CHSV(ledColor, 255, 255));
    }
    FastLED.show();
}