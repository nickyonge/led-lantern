#ifndef SLEEP_H
#define SLEEP_H

#include <Arduino.h>
#include <avr/sleep.h>

#include "main.h"
#include "input.h"

#define USE_SLEEP_TIMER

#ifdef USE_SLEEP_TIMER

// one second, taking DELAY_INTERVAL into account
constexpr int secondInterval = 1000 / DELAY_INTERVAL;

#define SECONDS_UNTIL_SLEEP 5
#define MINUTES_UNTIL_SLEEP 0

#endif // USE_SLEEP_TIMER

void setupSleep();
void loopSleep();

// resets the countdown timer for the device going to sleep
void resetSleepTimer();
// puts device to sleep, until any interrupt is received
void goToSleep();

#endif // SLEEP_H