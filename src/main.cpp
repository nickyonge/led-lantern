#include "main.h"

// #define NUM_LEDS_TEST 6

// CRGB ledsTest[NUM_LEDS_TEST];
void setup()
{
    // call class-specific setup functions
    setupSaveData();
    setupSleep();
    setupInput();
    setupLEDs(); // setup LEDs last (after Input)

    // FastLED.setBrightness(64);
    // FastLED.addLeds<WS2812B, 5, GRB>(ledsTest, NUM_LEDS_TEST);
    // for (int i = 1; i < NUM_LEDS_TEST; i++)
    // {
    //     // float f = 255.0 * (float)(i / NUM_LEDS_TEST);
    //     // int hue = (int)constrain((int)f,0,255);
    //     int hue = (int)(255.0 * (float)((float)i / NUM_LEDS_TEST));
    //     ledsTest[i] = CRGB(CHSV(hue, 255, 255));
    // }
    // ledsTest[0] = CRGB::Black;
    // FastLED.show();
}

// int tempI = 0;
// bool tempB = false;

void loop()
{
    // tempI += DELAY_INTERVAL;
    // if (tempI >= 1000) {
    //     tempI -= 1000;
    //     ledsTest[0] = tempB ? CRGB::Red : CRGB::Black;
    //     tempB = !tempB;
    //     FastLED.show();
    // }

    loopInput();
    loopLEDs();
    loopSaveData();
    loopSleep();

    // loop interval
    // FastLED.delay(DELAY_INTERVAL);
    delay(DELAY_INTERVAL);
}