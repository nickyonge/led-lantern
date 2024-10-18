#include "main.h"

void setup()
{
    // call class-specific setup functions
    setupLEDs();
    setupInput();
    setupSleep();
}

void loop()
{
    loopInput();

    // 1ms loop interval
    delay(1);
}