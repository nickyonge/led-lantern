#include "leds.h"

static bool savedLEDsThisSession = false; // on first save, ensure LEDs are updated correctly only once

CRGB leds[NUM_LEDS];
static int ledColor = DATA_DEFAULT_LED_HUE; // current LED HSV hue

static CRGB colorsArray[NUM_LEDS];

#ifndef DISABLE_ANIMATION
bool clearLEDs = false;
bool animate = true;
bool reverseAnimDirection = false;
int animTimer = 0;
#endif

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

void loopLEDs()
{
#ifndef DISABLE_ANIMATION
    // run animation
    if (animate)
    {
        // check timer
        animTimer++;
        if (animTimer >= animInterval)
        {
            // reset anim timer, and animate LEDs
            animTimer = 0;
            animateLEDs();
        }
    }
#endif
}

void updateLEDs()
{
#ifndef DISABLE_ANIMATION
    // if (clearLEDs)
    // {
    //     for (int i = 0; i < NUM_LEDS; i++)
    //     {
    //         colorsArray[i] = CRGB::Black;
    //     }
    //     clearLEDs = false;
    // }
    // else if (!animate)
    if (!animate)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            colorsArray[i] = CRGB(CHSV(ledColor, 255, 255));
        }
    }
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = colorsArray[i];
    }
#else
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(CHSV(ledColor, 255, 255));
    }
#endif
    FastLED.show();
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

#ifndef DISABLE_ANIMATION
void animateLEDs()
{
    random16_add_entropy(random());
    // Array of temperature readings at each simulation cell
    static uint8_t heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++)
    {
        // Scale the heat value from 0-255 down to 0-240
        int pixelnumber;
        if (reverseAnimDirection)
        {
            pixelnumber = (NUM_LEDS - 1) - j;
        }
        else
        {
            pixelnumber = j;
        }
        colorsArray[pixelnumber] = CRGB(CHSV(ledColor, 255, heat[j]));
    }
}
#endif

void clearLEDLocalData()
{
    // clear buffer + push to strip (fixes lingering bugs in data line)
    // per: https://www.reddit.com/r/FastLED/comments/dd950a/clear_turn_off_all_leds_before_void_loop_starts/
    // FastLED.clear(true);
    // NOTE: FastLED.clear(true) seems to add add'l size to the build, causing it to fail.
    //       Even tho the below is functionally identical, this does NOT break the build.
    // clearLEDs = true;
    // updateLEDs();
    // FastLED.clearData();
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