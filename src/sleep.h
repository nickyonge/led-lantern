#ifndef SLEEP_H
#define SLEEP_H

#include <Arduino.h>
#include <avr/sleep.h>

void setupSleep();

void goToSleep(); // puts device to sleep, until any interrupt is received

#endif