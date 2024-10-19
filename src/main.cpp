#include "main.h"

void setup()
{
    // call class-specific setup functions
    setupSaveData();
    setupSleep();
    setupInput();
    setupLEDs(); // setup LEDs last (after Input)
}

void loop()
{
    loopInput();
    loopLEDs();
    loopSaveData();
    loopSleep();

    // 1ms loop interval
    delay(1);
}