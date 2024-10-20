#ifndef SLEEP_H
#define SLEEP_H

#include <Arduino.h>
#include <avr/sleep.h>

#include "main.h" // note: importing main also imports other classes needed to call wake/sleep

#define USE_SLEEP_TIMER

#ifdef USE_SLEEP_TIMER

#define SECONDS_MAX 60  // 60s = 1 minute (it'd be bonkers to change this, but hey, maybe you just hate counting in Base60?)
#define MINUTES_MAX 240 // 240mins = 4 hours (prevent byte overflow at 255)

#define MINUTES_UNTIL_SLEEP 2  // how many minutes (plus given seconds) until device goes to sleep?
#define SECONDS_UNTIL_SLEEP 30 // how many seconds (after minutes target is reached) until device goes to sleep?

#endif // USE_SLEEP_TIMER

void setupSleep();
void loopSleep();

// Resets the countdown timer for the device going to sleep
void resetSleepTimer();
// Puts device to sleep, until any interrupt is received.
// NOTE: if no interrupts are set up, THIS FUNCTIONALLY DISABLES THE DEVICE. BE CAREFUL!
void goToSleep();

// error checks for defined values
#if MINUTES_UNTIL_SLEEP > MINUTES_MAX
#error "Max minutes exceeds minutes until sleep, device will never get a chance to rest!"
#endif
#if SECONDS_UNTIL_SLEEP > SECONDS_MAX
#error "Seconds until sleep exceeds max seconds increment (which should be 60), device will never get a chance to rest!"
#endif
#if MINUTES_MAX > 255 || SECONDS_MAX > 255 || MINUTES_UNTIL_SLEEP > 255 || SECONDS_UNTIL_SLEEP > 255
#error "Minutes and/or seconds sleep timers and/or max limits exceed 255. They are byte values, ensure they're 255 or less!"
#endif

#endif // SLEEP_H