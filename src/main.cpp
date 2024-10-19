#include "main.h"

void setup()
{
    // call class-specific setup functions
    setupSleep();
    setupInput();
    setupLEDs();// setup LEDs last (after Input)
}

void loop()
{
    loopInput();

    // 1ms loop interval
    delay(1);
}