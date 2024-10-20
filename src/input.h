#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

#include <RotaryEncoder.h>

#include "pindef.h"
#include "sleep.h"
#include "leds.h"

#define ENC_ROTATION_WAKES_DEVICE // rotating the encoder will wake the device. otherwise, it must be clicked
#define POLL_ENCODER_INTERRUPTS   // poll the encoder during clk/data pin interrupts
#define POLL_ENCODER_LOOP         // poll the encoder during loopInput cycle
#define USE_ENCODER_ACCELERATION  // should encoder speed be accelerated?

#define USE_ENCODER_SWITCH_LOGIC // use in-loop logic for encoder switch, beyond just interrupt
#ifdef USE_ENCODER_SWITCH_LOGIC
// #define ENCODER_SWITCH_LOGIC_POLL      // process encoder switch logic by reading pins in loopInput()
#define ENCODER_SWITCH_LOGIC_INTERRUPT // process encoder switch logic by waiting for an interrupt on the switch pin
#endif                                 // note: defining both POLL and INTERRUPT can result in the signal sending multiple times

#ifdef USE_ENCODER_ACCELERATION
// accel per https://github.com/mathertel/RotaryEncoder/blob/master/examples/AcceleratedRotator/AcceleratedRotator.ino

// the maximum acceleration is 10 times.
constexpr float maxAccel = 5; // default 10
// at 200ms or slower, there should be no acceleration. (factor 1)
constexpr float longCutoff = 50; // default 50
// at 5 ms, we want to have maximum acceleration (factor maxAccel)
constexpr float shortCutoff = 5; // default 5

// To derive the calc. constants, compute as follows:
// On an x(ms) - y(factor) plane resolve a linear formular factor(ms) = a * ms + b;
// where  f(4)=10 and f(200)=1

constexpr float a = (maxAccel - 1) / (shortCutoff - longCutoff); // 0.2
constexpr float b = 1 - (longCutoff * a);                        // -9

#else
#define ENC_NONACCEL_MULTIPLIER 4 // if defined, multiply encoder delta for LED shift by this
#endif

#define ENC_LATCH_MODE RotaryEncoder::LatchMode::FOUR3 // latch mode to use for rotary encoder

void setupInput();
void loopInput();

// callback for enc switch pin interrupt
void interruptSwitch();
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
// callback for enc data pins interrupt (see )
void interruptEncoder();
#endif

void inputSwitch();
void inputEncoder();

// call from sleep.h when device is put to sleep (to disable input interrupts)
void sleepInput();
// call from sleep.h when device wakes up (to re-enable input interrupts)
void wakeInput();

// error check for no encoder polling
#if !defined(POLL_ENCODER_INTERRUPTS) && !defined(POLL_ENCODER_LOOP)
#error "Neither POLL_ENCODER_INTERRUPTS nor POLL_ENCODER_LOOP are defnied - at least ONE should be active!"
#endif
// error check for invalid switch logic
#if defined(USE_ENCODER_SWITCH_LOGIC) && !defined(ENCODER_SWITCH_LOGIC_POLL) && !defined(ENCODER_SWITCH_LOGIC_INTERRUPT)
#error "USE_ENCODER_SWITCH_LOGIC is defined, but logic is processed neither by polling pins nor reading interrupt. Disable switch logic, or define a logic source"
#endif

#endif // INPUT_H