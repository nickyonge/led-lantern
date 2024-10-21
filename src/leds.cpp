#include "leds.h"

static bool savedLEDsThisSession = false; // on first save, ensure LEDs are updated correctly only once

volatile bool queueUpdateLEDs = false; // if true, calls `updateLEDs()` at the start of the next `loopLEDs` cycle

CRGB leds[NUM_LEDS];
static int ledColor = DATA_DEFAULT_LED_HUE; // current LED HSV hue

static CRGB colorsArray[NUM_LEDS];
bool clearLEDs = false;

#ifdef ENABLE_ANIMATION
bool animate = true;
bool reverseAnimDirection = false;
int animTimer = 0;
#endif

//
// ------------------------------------------------------------ [  SETUP AND LOOP  ] ---------
//

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
    if (queueUpdateLEDs)
    {
        updateLEDs();
    }
#ifdef ENABLE_ANIMATION
    // run animation
    if (animate)
    {
        // check timer
        animTimer += DELAY_INTERVAL;
        if (animTimer >= ANIM_FPS)
        {
            // reset anim timer, and animate LEDs
            animTimer -= ANIM_FPS;
            animateLEDs();
        }
    }
#endif
}

//
// ------------------------------------------------------------ [  LED DISPLAY LOGIC  ] ---------
//

void updateLEDs()
{
#ifdef ENABLE_ANIMATION
    if (clearLEDs)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            colorsArray[i] = CRGB::Black;
        }
        clearLEDs = false;
    }
    else if (!animate)
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
    if (clearLEDs)
    {
        clearLEDs = false;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB::Black;
        }
    }
    else
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(CHSV(ledColor, 255, 255));
        }
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

void jumpLEDColor()
{
    shiftLEDColor(128);
}

void testLEDColor()
{
    ledColor = 0;
    updateLEDs();
}

#ifdef ENABLE_ANIMATION
void animateLEDs()
{
}
#endif

//
// ------------------------------------------------------------ [  SLEED/WAKE  ] ---------
//

void sleepLEDs()
{
    // clear LEDs so the entire strip turns black
    clearLEDLocalData();
}
void wakeLEDs()
{
    // queue an update to wake the LEDs so the strip is displayed again
    queueUpdateLEDs = true;
}

//
// ------------------------------------------------------------ [  SAVE/LOAD DATA  ] ---------
//

void clearLEDLocalData()
{
    // clear buffer + push to strip (fixes lingering bugs in data line)
    // per: https://www.reddit.com/r/FastLED/comments/dd950a/clear_turn_off_all_leds_before_void_loop_starts/
    // FastLED.clear(true);
    // NOTE: FastLED.clear(true) seems to add add'l size to the build, causing it to fail.
    //       Even tho the below is functionally identical, this does NOT break the build.
    clearLEDs = true;
    updateLEDs();
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