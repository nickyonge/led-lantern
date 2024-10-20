#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#define DELAY_INTERVAL 1 // delay in ms per loop() cycle
// NOTE: DELAY_INTERVAL value should be such that 1000/DELAY_INTERVAL results in a whole number.
//       Otherwise, sleep timer timing will be off.

// One second in num of cycle ticks, taking DELAY_INTERVAL into account
constexpr int secondInterval = 1000 / DELAY_INTERVAL;

#include "leds.h"
#include "input.h"
#include "sleep.h"
#include "savedata.h"

#endif // MAIN_H