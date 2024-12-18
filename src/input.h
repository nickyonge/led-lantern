#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

#include <RotaryEncoder.h>

#include "pindef.h"
#include "sleep.h"
#include "leds.h"
#include "byteMath.h"

#define ENABLE_INPUT // is input system enabled?
#ifdef ENABLE_INPUT

#define LOOP_INTERVAL_INPUT 20 // how many ms in between loop() ticks for this class?

#define ENC_SWITCH_WAKES_DEVICE   // clicking the encoder switch will wake the device
#define ENC_ROTATION_WAKES_DEVICE // rotating the encoder will wake the device. otherwise, it must be clicked
#define POLL_ENCODER_INTERRUPTS   // poll the encoder rotation during clk/data pin interrupts
#define POLL_ENCODER_LOOP         // poll the encoder rotation during loopInput cycle

// #define ENC_ROTATION_ACCELERATION // should encoder speed be accelerated? - Good functionality, but nearly 1kB flash mem

#define USE_ENCODER_SWITCH_LOGIC // use in-loop logic for encoder switch, beyond just interrupt?
#ifdef USE_ENCODER_SWITCH_LOGIC
#define ENCODER_SWITCH_WAKE_INPUT_DELAY 10 // time in ms to delay reading switch input upon waking
#define ENCODER_SWITCH_INPUT_BUFFER 10     // time in ms to buffer any input received on the encoder switch
#define ENCODER_SWITCH_LOGIC_POLL          // process encoder switch logic by reading pins in loopInput() (requiured for switch held timeouts)
#define ENCODER_SWITCH_LOGIC_INTERRUPT     // process encoder switch logic by waiting for an interrupt on the switch pin
// #define ENCODER_SWITCH_JUMPS_LEDS          // switch input causes LEDs to jump halfway across the colour spectrum
#ifdef ENCODER_SWITCH_LOGIC_POLL
#define ENC_HELD_SLEEP_TIMEOUT 2000    // how long, in ms, holding the switch down takes to put the device to sleep. 0 = never. Requires poll logic
#define ENC_HELD_ADJUST_BRIGHTNESS 100 // how long, in ms, after holding the switch down, will rotating the encoder result adjusting brightness?
#if defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0
#define ENC_ADJUST_BRIGHTNESS_AMT_DISABLES_SLEEP 8 // how much must the brightness value be adjusted before the sleep timeout is disabled until btn release?
#endif
#endif // end ENCODER_SWITCH_LOGIC_POLL
#endif // USE_ENCODER_SWITCH_LOGIC

#ifdef ENC_ROTATION_ACCELERATION
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
#define ENC_NONACCEL_MULTIPLIER 6 // if defined, multiply encoder delta for LED shift by this
#endif

#define ENC_LATCH_MODE RotaryEncoder::LatchMode::FOUR3 // latch mode to use for rotary encoder

#endif // ENABLE_INPUT

void setupInput();
void loopInput();

#ifdef ENABLE_INPUT
// callback for enc switch pin interrupt
void interruptSwitch();
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
// callback for enc data pins interrupt (see )
void interruptEncoder();
#endif
#endif

// call from sleep.h when device is put to sleep (to disable input interrupts)
void sleepInput();
// call from sleep.h when device wakes up (to re-enable input interrupts)
void wakeInput();
// Check to see if the wakeup was valid - if so, continue wakeup - if not, put device back to sleep.
// Wakeup is INVALID if it's via the SWITCH interrupt,
bool validWakeUp();

#ifdef ENABLE_INPUT
// error check for no encoder polling
#if !defined(POLL_ENCODER_INTERRUPTS) && !defined(POLL_ENCODER_LOOP)
#error "Neither POLL_ENCODER_INTERRUPTS nor POLL_ENCODER_LOOP are defnied - at least ONE should be active!"
#endif
// error check for invalid switch logic
#if defined(USE_ENCODER_SWITCH_LOGIC) && !defined(ENCODER_SWITCH_LOGIC_POLL) && !defined(ENCODER_SWITCH_LOGIC_INTERRUPT)
#error "USE_ENCODER_SWITCH_LOGIC is defined, but logic is processed neither by polling pins nor reading interrupt. Disable switch logic, or define a logic source"
#endif
// error check for impossible to wake device
#if !defined(ENC_SWITCH_WAKES_DEVICE) && !defined(ENC_ROTATION_WAKES_DEVICE)
#error "Uh-oh, neither clicking nor rotating the encoder will wake the device. It's gonna sleep forever! One must be defined"
#endif
#endif

#endif // INPUT_H