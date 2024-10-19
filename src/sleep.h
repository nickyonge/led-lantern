#ifndef SLEEP_H
#define SLEEP_H

#include <Arduino.h>
#include <avr/sleep.h>

#include "input.h"

void setupSleep();
void loopSleep();

void goToSleep(); // puts device to sleep, until any interrupt is received

#endif // SLEEP_H