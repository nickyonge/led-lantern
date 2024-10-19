#include "sleep.h"

void setupSleep()
{
    // prep correct sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // info on sleep modes: https://onlinedocs.microchip.com/oxy/GUID-A834D554-5741-41A3-B5E1-35ED7CD8250A-en-US-5/GUID-35CAFA19-CA93-4B3E-AEE3-481B8542FE94.html
}

void goToSleep()
{
    sleepInput();        // put input system to sleep
    sleep_enable();      // enable sleep bit
    sleep_bod_disable(); // disable brownout detection
    sei();               // ensure interrupts are active
    sleep_cpu();         // begin sleep mode

    // >>> device automatically wakes on any interrupt <<<

    sleep_disable(); // disable sleep bit
    sei();           // re-enable interrupts again (failsafe)
    wakeInput();     // wake up input system
}