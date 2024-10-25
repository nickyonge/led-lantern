#ifndef SLEEP_H
#define SLEEP_H

#include <Arduino.h>
#include <avr/sleep.h>

#include "main.h" // note: importing main also imports other classes needed to call wake/sleep

#define ENABLE_SLEEP // is sleep system enabled?

#ifdef ENABLE_SLEEP

#define LOOP_INTERVAL_SLEEP 1000 // how many ms in between loop() ticks for this class?

#define USE_SLEEP_TIMER // will the device automatically sleep after a certain time without interaction? 

#ifdef USE_SLEEP_TIMER

#define CYCLES_PER_SEC 1000 // ms equivalent, 1000 `loop()` ticks = 1 second (refer to `main.h` / `DELAY_INTERVAL`)
#define SECONDS_PER_MIN 60  // 60s = 1 minute (it'd be bonkers to change this, but hey, maybe you just hate counting in Base60?)
#define MINUTES_MAX 240     // 240mins = 4 hours (prevent byte overflow at 255)

#define MINUTES_UNTIL_SLEEP 0 // how many minutes (plus given seconds) until device goes to sleep?
#define SECONDS_UNTIL_SLEEP 5 // how many seconds (after minutes target is reached) until device goes to sleep?

#endif // ifdef USE_SLEEP_TIMER
#endif // ifdef ENABLE_SLEEP

void setupSleep();
void loopSleep();

// Resets the countdown timer for the device going to sleep
void resetSleepTimer();
// Puts device to sleep, until any interrupt is received.
//
// IMPORTANT: Do NOT call `goToSleep` from an interrupt or otherwise outside the main cycle.
// This may cause issues when putting other classes to sleep.
//
// ~ ~ ~ ~ ~
//
// HOWEVER: Always assume that the WAKE cycle IS coming from an interrupt!
// Any properties there must be volatile. Set flags for the next `loop()` cycle.
//
// ~ ~ ~ ~ ~
//
// ALSO: if no interrupts are set up, THIS FUNCTIONALLY DISABLES THE DEVICE. BE CAREFUL!
void goToSleep();

// error checks for defined values
#if MINUTES_UNTIL_SLEEP > MINUTES_MAX
#error "Max minutes exceeds minutes until sleep, device will never get a chance to rest!"
#endif
#if SECONDS_UNTIL_SLEEP > SECONDS_PER_MIN
#error "Seconds until sleep exceeds max seconds increment (which should be 60), device will never get a chance to rest!"
#endif
#if MINUTES_MAX > 255 || SECONDS_MAX > 255 || MINUTES_UNTIL_SLEEP > 255 || SECONDS_UNTIL_SLEEP > 255
#error "Minutes and/or seconds sleep timers and/or max limits exceed 255. They are byte values, ensure they're 255 or less!"
#endif

#endif // SLEEP_H