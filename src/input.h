#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

#include <RotaryEncoder.h>

#include "pindef.h"
#include "leds.h"

#define USE_ENCODER_SWITCH_LOGIC  // use in-loop logic for encoder switch, beyond just interrupt
#define ENC_ROTATION_WAKES_DEVICE // rotating the encoder will wake the device. otherwise, it must be clicked
#define POLL_ENCODER_INTERRUPTS   // poll the encoder during clk/data pin interrupts
#define POLL_ENCODER_LOOP         // poll the encoder during loopInput cycle
#define USE_ENCODER_ACCELERATION  // should encoder speed be accelerated?

#ifndef USE_ENCODER_ACCELERATION
#define ENC_NONACCEL_MULTIPLIER 4
#endif

#define ENC_LATCH_MODE RotaryEncoder::LatchMode::FOUR3

void setupInput();
void loopInput();

// callback for enc switch pin interrupt
void interruptSwitch();
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
// callback for enc data pins interrupt (see )
void interruptEncoder();
#endif

// call from sleep.h when device is put to sleep (to disable input interrupts)
void sleepInput();
// call from sleep.h when device wakes up (to re-enable input interrupts)
void wakeInput();

// error check for no encoder polling
#if !defined(POLL_ENCODER_INTERRUPTS) && !defined(POLL_ENCODER_LOOP)
#error "Neither POLL_ENCODER_INTERRUPTS nor POLL_ENCODER_LOOP are defnied - at least ONE should be active!"
#endif

#endif // INPUT_H