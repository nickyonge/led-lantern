#include "leds.h"

CRGB leds[NUM_LEDS];

static int ledColor = DATA_DEFAULT_LED_HUE; // current LED HSV hue

static bool savedLEDsThisSession = false; // on first save, ensure LEDs are updated correctly only once

void setupLEDs()
{
    // prep LEDs
    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.addLeds<CHIPSET, PIN_LED_DATA, GRB>(leds, NUM_LEDS);

    // clear LED local data
    clearLEDLocalData();

    // load intial values
    loadLEDData();

    // initial update (failsafe, technically called in main as well)
    updateLEDs();
}

void shiftLEDColor(int delta)
{
    ledColor += delta;
    while (ledColor > 255)
    {
        ledColor -= 256;
    }
    while (ledColor < 0)
    {
        ledColor += 256;
    }
    updateLEDs();
    saveLEDData();
}

void updateLEDs()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(CHSV(ledColor, 255, 255));
    }
    FastLED.show();
}

void clearLEDLocalData()
{
    // clear buffer + push to strip (fixes lingering bugs in data line)
    // per: https://www.reddit.com/r/FastLED/comments/dd950a/clear_turn_off_all_leds_before_void_loop_starts/
    // FastLED.clear(true);
    // NOTE: FastLED.clear(true) seems to add add'l size to the build, causing it to fail.
    //       Even tho the below is functionally identical, this does NOT break the build.
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    FastLED.clearData();
}

void loadLEDData()
{
    if (!hasSaved())
    {
        // never saved before, use initial save data
        return;
    }
    // load values
    ledColor = getSaveData()->color;
}
void saveLEDData()
{
    getSaveData()->color = ledColor;
    queueSaveData();
    if (!savedLEDsThisSession)
    {
        savedLEDsThisSession = true;
        updateLEDs();
    }
}