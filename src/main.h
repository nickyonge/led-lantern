#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

// How many ticks in one second?
//
// In theory this should be 1000, but for reasons I cannot for the life of me uncover,
// delay(1000) takes about 2.5s in this project.
// So, for now, 1000/2.5 = 400, and subjectively
// 400 is too fast, so we use 420 =_=;;;;
// it even varies between MCUs... ugh.
// #define CYCLES_SECOND 420 

#define CYCLES_SECOND 1000

#define DELAY_INTERVAL 1 // delay in ms per loop() cycle
// NOTE: DELAY_INTERVAL value should be such that 
//       CYCLES_SECOND/DELAY_INTERVAL results in a whole number.
//       Otherwise, sleep timer timing will be off.

#include "leds.h"
#include "input.h"
#include "sleep.h"
#include "savedata.h"

#endif // MAIN_H