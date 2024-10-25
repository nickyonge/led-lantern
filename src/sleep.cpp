#include "sleep.h"

#ifdef USE_SLEEP_TIMER

static int _loopIntervalSleep = 0; // timer to keep track of loop() intervals for this class

byte secondsIdle = 0;
byte minutesIdle = 0;

#endif

void setupSleep()
{
#ifdef ENABLE_SLEEP
    // prep correct sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
// info on sleep modes: https://onlinedocs.microchip.com/oxy/GUID-A834D554-5741-41A3-B5E1-35ED7CD8250A-en-US-5/GUID-35CAFA19-CA93-4B3E-AEE3-481B8542FE94.html
#endif
}

void loopSleep()
{
#ifdef USE_SLEEP_TIMER
    // check savedata loop delay
#if defined(LOOP_INTERVAL_SLEEP) && LOOP_INTERVAL_SLEEP > 1
    _loopIntervalSleep += DELAY_INTERVAL;
    if (_loopIntervalSleep >= LOOP_INTERVAL_SLEEP)
    {
        _loopIntervalSleep -= LOOP_INTERVAL_SLEEP;
    }
    else
    {
        return;
    }
#else
#error "LOOP_INTERVAL_SLEEP is not used, it should be set to 1000, as it's used to check how many ms are in one second"
#endif
    // passed initial loop check, increment seconds and minutes as needed
    // one second passed
    secondsIdle++;
    if (secondsIdle >= SECONDS_PER_MIN)
    {
        // seconds overflow, increment minute
        secondsIdle -= SECONDS_PER_MIN;
        minutesIdle++;
        // ensure minutesIdle does not exceed given limit
        if (minutesIdle > MINUTES_MAX)
        {
            minutesIdle = MINUTES_MAX; // cap at MINUTES_MAX
        }
    }
    // check for sleep timer limits
    if (minutesIdle >= MINUTES_UNTIL_SLEEP && secondsIdle >= SECONDS_UNTIL_SLEEP)
    {
        // device has reached sleep mode timeout
        goToSleep();
    }
#endif
}

void resetSleepTimer()
{
#ifdef USE_SLEEP_TIMER
    secondsIdle = 0;
    minutesIdle = 0;
#endif
}

void goToSleep()
{
#ifdef ENABLE_SLEEP
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
#endif
}