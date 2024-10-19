#include "sleep.h"

#ifdef USE_SLEEP_TIMER
int loopsIdle = 0;
byte secondsIdle = 0;
byte minutesIdle = 0;
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
    // increment idle timer
    loopsIdle++;
    if (loopsIdle >= secondInterval)
    {
        // increment seconds and minutes as needed
        loopsIdle = 0;
        secondsIdle++;
        if (secondsIdle >= SECONDS_MAX)
        {
            // seconds overflow, increment minute
            secondsIdle = 0;
            minutesIdle++;
            // ensure minutesIdle does not exceed given limit
            if (minutesIdle > MINUTES_MAX)
            {
                minutesIdle = MINUTES_MAX;
            }
        }
        // check for sleep timer limits
        if (minutesIdle >= MINUTES_UNTIL_SLEEP && secondsIdle >= SECONDS_UNTIL_SLEEP)
        {
            // device has reached sleep mode timeout
            goToSleep();
        }
    }
#endif
}

void resetSleepTimer()
{
#ifdef USE_SLEEP_TIMER
    loopsIdle = 0;
    secondsIdle = 0;
    minutesIdle = 0;
#endif
}

void goToSleep()
{
    // 1) Sleep other classes as necessary
    sleepInput(); // put input system to sleep
    sleepLEDs();  // put LED display to sleep

    // 2) Prep device for sleep mode
    resetSleepTimer();   // reset sleep timing values
    sleep_enable();      // enable sleep bit
    sleep_bod_disable(); // disable brownout detection
    sei();               // ensure interrupts are active

    // 3) put device to sleep
    sleep_cpu(); // begin sleep mode

    // >>> device automatically wakes on any interrupt <<<

    // 4) Device woken up by interrupt, disable sleep mode
    sleep_disable(); // disable sleep bit
    sei();           // re-enable interrupts again (failsafe)

    // 5) Wake other classes as necessary
    wakeInput(); // wake up input system
    wakeLEDs();  // wake up LED display
}