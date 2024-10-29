#include "leds.h"

static byte _loopIntervalLEDs = 0; // timer to keep track of loop() intervals for this class

static bool savedLEDsThisSession = false; // on first save, ensure LEDs are updated correctly only once

volatile bool queueUpdateLEDs = false; // if true, calls `updateLEDs()` at the start of the next `loopLEDs` cycle

CRGB leds[NUM_LEDS];
static byte ledColor = DATA_DEFAULT_LED_HUE; // current LED HSV hue

static byte ledBrightness = 255; // 0-255, 0 = `LED_MIN_BRIGHTNESS`, 255 = 255, capped by `FastLED.setBrightness(LED_MAX_BRIGHTNESS)`

bool clearLEDs = false; // should LEDs be cleared on next updateLEDs() call?

#ifdef ENABLE_ANIMATION
Random16 rng; // random number generator
ByteDrifter byteDrifter(rng);
static CRGB colorsArray[NUM_LEDS];
bool animate = true;
bool reverseAnimDirection = false;
int animTimer = 0;
#endif

#ifdef DEBUG_FLASH_LED_0
int debugFlashTimer = 0;
bool debugFlashOn = false;
#endif

//
// ------------------------------------------------------------ [  SETUP AND LOOP  ] ---------
//

void setupLEDs()
{
// prep LEDs
#ifdef CALL_FASTLED_METHODS
    FastLED.setBrightness(LED_MAX_BRIGHTNESS);
    FastLED.addLeds<CHIPSET, PIN_LED_DATA, GRB>(leds, NUM_LEDS);

#ifdef LED_MAX_MILLIAMP_DRAW
    FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_MAX_MILLIAMP_DRAW);
#endif
#endif

    // clear LED local data
    clearLEDLocalData();

    // load intial values
    loadLEDData();

// init random seed
#ifdef ENABLE_ANIMATION
    rng.setSeed(analogRead(PIN_RANDOMSEED));
#endif

    // initial update (failsafe, technically called in main as well)
    queueUpdateLEDs = true;
    // updateLEDs();
}

void loopLEDs()
{
// check LEDs loop delay
#if defined(LOOP_INTERVAL_LEDS) && LOOP_INTERVAL_LEDS > 1
    _loopIntervalLEDs += DELAY_INTERVAL;
    if (_loopIntervalLEDs >= LOOP_INTERVAL_LEDS)
    {
        _loopIntervalLEDs -= LOOP_INTERVAL_LEDS;
    }
    else
    {
        return;
    }
#endif
// check debug flash
#ifdef DEBUG_FLASH_LED_0
    debugFlashTimer += LOOP_INTERVAL_LEDS;
    if (debugFlashTimer >= 1000)
    {
        debugFlashTimer -= 1000;
        debugFlashOn = !debugFlashOn;
        queueUpdateLEDs = true;
    }
#endif
    // check for queued LED update (probably from a wake cycle, or debug flash)
    if (queueUpdateLEDs)
    {
        updateLEDs();
    }
#ifdef ENABLE_ANIMATION
    // run animation
    if (animate)
    {
        // check timer
        animTimer += LOOP_INTERVAL_LEDS;
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
    // first, check if we're clearing LEDs
    if (clearLEDs)
    {
        for (byte i = 0; i < NUM_LEDS; i++)
        {
#ifdef ENABLE_ANIMATION
            colorsArray[i] = CRGB::Black;
#endif
            leds[i] = CRGB::Black;
        }
    }
    else
    {
        // not clearing LEDs, check if anim is enabled
#ifdef ENABLE_ANIMATION
        // animation is enabled, check if active
        if (!animate)
        {
            // anim is active
            for (byte i = 0; i < NUM_LEDS; i++)
            {
                colorsArray[i] = CRGB(CHSV(ledColor, 255, ledBrightness));
            }
        }
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = colorsArray[i];
        }
#else
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(CHSV(ledColor, 255, ledBrightness));
        }
#endif
    }
    // check for debug LED flashing
#ifdef DEBUG_FLASH_LED_0
    leds[0] = debugFlashOn && !clearLEDs ? CRGB::Red : CRGB::Black;
#endif
// update FastLED strip
#ifdef CALL_FASTLED_METHODS
    FastLED.show();
#endif
    // reset clear LEDs
    clearLEDs = false;
}

void shiftLEDColor(byte delta)
{
    if (delta == 0)
    {
        return;
    }
    ledColor += delta;
    updateLEDs();
    saveLEDData();
}

void shiftLEDBrightness(byte delta)
{
    if (delta == 0)
    {
        return;
    }
    ledBrightness = addByte(ledBrightness, delta);
    if (ledBrightness < LED_MIN_BRIGHTNESS)
    {
        ledBrightness = LED_MIN_BRIGHTNESS;
    }
    updateLEDs();
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
    // animation step
    updateLEDs();
}
#endif

byte getLEDBrightness()
{
    return 255; // TEMP

    return map(ledBrightness, 0, 255, LED_MIN_BRIGHTNESS, 255);
}

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
#ifdef CALL_FASTLED_METHODS
    FastLED.clearData();
#endif
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
    ledBrightness = getSaveData()->brightness;
}
void saveLEDData()
{
    getSaveData()->color = ledColor;
    getSaveData()->brightness = ledBrightness;
    queueSaveData();
    if (!savedLEDsThisSession)
    {
        savedLEDsThisSession = true;
        updateLEDs();
    }
}