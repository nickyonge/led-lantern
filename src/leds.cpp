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
static CRGB colorsArray[NUM_LEDS];
int animTimer = 0;
#ifdef ADVANCED_ANIMATION
ByteDrifter byteDrifter(rng);
#else
byte brightnessFalloffValue = BRIGHTNESS_FALLOFF_MAX;  // actual value that brightness falls off
byte brightnessFalloffTarget = BRIGHTNESS_FALLOFF_MAX; // target value for brightness falloff
byte brightnessInterval = 0;                           // ticks (in anim fps) until next brightness falloff randomization
byte brightnessSpeed = BRIGHTNESS_FALLOFF_SPEED_MIN;   // speed (in byte units/frame) brightnessFalloffValue moves to target
#endif
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
    FastLED.addLeds<CHIPSET, PIN_LED_DATA, RGB_ORDER>(leds, NUM_LEDS);

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
#ifdef ENABLE_ANIMATION
    // run animation
    // check timer
    animTimer += LOOP_INTERVAL_LEDS;
    if (animTimer >= ANIM_FPS)
    {
        // reset anim timer, and animate LEDs
        animTimer -= ANIM_FPS;
        animateLEDs();
    }
#endif
    // check for queued LED update (probably from a wake cycle, or debug flash)
    if (queueUpdateLEDs)
    {
        updateLEDs();
    }
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
        // animation is enabled
#ifdef ADVANCED_ANIMATION
        // reset iteration HERE, so we don't duplicate iterations if we only change LED color
        // (and thus cause iteration decay multiple times per animation frame)
        byteDrifter.resetIteration();
#else
        byte brightness = ledBrightness;
#endif
        for (byte i = 0; i < NUM_LEDS; i++)
        {
#ifdef ADVANCED_ANIMATION
            // advanced animation using byteDrifter for brightness
            byte brightness = byteDrifter.getValue();
            // apply colour if brightness exceeds min value, otherwise, set black
            colorsArray[i] = brightness >= LED_MIN_BRIGHTNESS ? CRGB(CHSV(ledColor, 255, brightness)) : CRGB::Black;
#else
            colorsArray[i] = CRGB(CHSV(ledColor, 255, brightness));
            brightness = subtractByte(brightness, brightnessFalloffValue);
#endif
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
    // reset update LEDs queue
    queueUpdateLEDs = false;
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
#ifdef ADVANCED_ANIMATION
    byteDrifter.tick(false);
#else
    // decrement interval
    if (brightnessInterval == 0)
    {
        // interval has reached zero, re-randomize target values
        uint16_t value = rng.get();     // generate a random number between 0-65535
        float p = value / (float)65535; // convert to float between 0.00 and 1.00
        // p = pow(p, BRIGHTNESS_CURVE_POWER); // multiply p to a power of itself to curve
        for (byte b = 0; b < BRIGHTNESS_CURVE_POWER; b++)
        {
            p *= p; // using pow used a bunch of space, just write it manually =_=
        }
        byte randomByte = byte((254.5 * p) + 0.5); // convert random 0.0-1.0 float to 0-255 byte
        // assign brightnessFalloffTarget, mapped from 0-255 to falloff min/max
        brightnessFalloffTarget = map(randomByte, 0, 255, BRIGHTNESS_FALLOFF_MIN, BRIGHTNESS_FALLOFF_MAX);
        // re-randomize speed and interval
        brightnessSpeed = rng.get(BRIGHTNESS_FALLOFF_SPEED_MIN, BRIGHTNESS_FALLOFF_SPEED_MAX);
        brightnessInterval = rng.get(BRIGHTNESS_FALLOFF_INTERVAL_MIN, BRIGHTNESS_FALLOFF_INTERVAL_MAX);
    }
    else
    {
        brightnessInterval--; // interval waiting, decrement
    }
    // move brightness falloff value to target
    if (brightnessFalloffValue < brightnessFalloffTarget)
    {
        // less than, add
        brightnessFalloffValue = addByte(brightnessFalloffValue, brightnessSpeed, brightnessFalloffTarget);
    }
    else if (brightnessFalloffValue > brightnessFalloffTarget)
    {
        // greater than, subtract
        brightnessFalloffValue = subtractByte(brightnessFalloffValue, brightnessSpeed, brightnessFalloffTarget);
    }
#endif
    updateLEDs();
}
#endif

// #define BRIGHTNESS_FALLOFF_SPEED_MIN 2
// #define BRIGHTNESS_FALLOFF_SPEED_MAX 12
// #define BRIGHTNESS_FALLOFF_INTERVAL_MIN 10
// #define BRIGHTNESS_FALLOFF_INTERVAL_MAX 60
// byte brightnessFalloffValue = 0;
// byte brightnessFalloffTarget = 0;
// byte brightnessInterval = 0;
// byte brightnessSpeed = 0;

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