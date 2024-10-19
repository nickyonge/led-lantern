#include "input.h"
#include <EnableInterrupt.h>

static volatile int encPos = 0;        // current position of rotary encoder
static bool encSwitch = false;         // is rotary encoder switch currently pressed?
volatile bool lastInterrupted = false; // was interrupt called before last loop cylce?

#ifdef POLL_ENCODER_INTERRUPTS
RotaryEncoder *encoder = nullptr;
#else
RotaryEncoder encoder(PIN_ENC_CLK, PIN_ENC_DAT, ENC_LATCH_MODE);
#endif

void setupInput()
{
// create encoder if necessary
#ifdef POLL_ENCODER_INTERRUPTS
    encoder = new RotaryEncoder(PIN_ENC_CLK, PIN_ENC_DAT, ENC_LATCH_MODE);
#endif
    // encoder switch pin
    pinMode(PIN_ENC_SWITCH, INPUT_PULLUP);
    // enable interrupt on enc switch pin
    enableInterrupt(PIN_ENC_SWITCH, interruptSwitch, FALLING);
    // check for encoder data interrupts
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
    enableInterrupt(PIN_ENC_DAT, interruptEncoder, CHANGE);
    enableInterrupt(PIN_ENC_CLK, interruptEncoder, CHANGE);
#endif
}

void interruptSwitch()
{
    lastInterrupted = true;
}

#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
void interruptEncoder()
{
#ifdef POLL_ENCODER_INTERRUPTS
    encoder->tick();
#endif
}
#endif

void loopInput()
{
    // check for interrupt since previous cycle
    if (lastInterrupted)
    {
        // yup, interrupt detected
        lastInterrupted = false;
    }

// read rotary encoder switch
#ifdef USE_ENCODER_SWITCH_LOGIC
    bool lastSwitch = encSwitch;
    encSwitch = !digitalRead(PIN_ENC_SWITCH); // NC switch, invert
    if (lastSwitch != encSwitch)
    {
        // switch state toggled, do stuff...
    }
#endif

    // update and read rotary encoder movement
#ifdef POLL_ENCODER_INTERRUPTS
#ifdef POLL_ENCODER_LOOP
    encoder->tick();
#endif
    int newPos = encoder->getPosition();
#else
#ifdef POLL_ENCODER_LOOP
    encoder.tick();
#endif
    int newPos = encoder.getPosition();
#endif

    // check for position change
    if (encPos != newPos)
    {
        // position changed, read movement delta
        int delta = encPos - newPos;

        // add acceleration or delta multiplier, as needed
#ifdef USE_ENCODER_ACCELERATION
#else
#ifdef ENC_NONACCEL_MULTIPLIER
        delta *= ENC_NONACCEL_MULTIPLIER;
#endif
#endif

        // update LED colour by delta amount
        shiftLEDColor(delta);
        encPos = newPos;
    }
}

void sleepInput()
{
#ifndef ENC_ROTATION_WAKES_DEVICE
#ifdef POLL_ENCODER_INTERRUPTS
    disableInterrupt(PIN_ENC_DAT);
    disableInterrupt(PIN_ENC_CLK);
#endif
#endif
}
void wakeInput()
{
#ifndef ENC_ROTATION_WAKES_DEVICE
#ifdef POLL_ENCODER_INTERRUPTS
    enableInterrupt(PIN_ENC_DAT, interruptEncoder, CHANGE);
    enableInterrupt(PIN_ENC_CLK, interruptEncoder, CHANGE);
#endif
#endif
}