#include "sleep.h"

#ifdef USE_SLEEP_TIMER

byte seconds_idle = 0;
byte minutes_idle = 0;
#endif

void setupSleep()
{
    // prep correct sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // info on sleep modes: https://onlinedocs.microchip.com/oxy/GUID-A834D554-5741-41A3-B5E1-35ED7CD8250A-en-US-5/GUID-35CAFA19-CA93-4B3E-AEE3-481B8542FE94.html
}

void loopSleep()
{
#ifdef USE_SLEEP_TIMER
    secondInterval;
#endif
}

void resetSleepTimer()
{
#ifdef USE_SLEEP_TIMER
#endif
}

void goToSleep()
{
    sleepInput();        // put input system to sleep
    sleepLEDs();         // put LED display to sleep
    sleep_enable();      // enable sleep bit
    sleep_bod_disable(); // disable brownout detection
    sei();               // ensure interrupts are active
    sleep_cpu();         // begin sleep mode

    // >>> device automatically wakes on any interrupt <<<

    sleep_disable(); // disable sleep bit
    sei();           // re-enable interrupts again (failsafe)
    wakeInput();     // wake up input system
    wakeLEDs();      // wake up LED display
}