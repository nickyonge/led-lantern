#include "input.h"
#include <EnableInterrupt.h>

static int encPos = 0;         // current position of rotary encoder
static bool encSwitch = false; // is rotary encoder switch currently pressed?

volatile bool interruptedBySwitch = false;  // was interrupt via enc switch called before last loop cylce?
volatile bool interruptedByEncoder = false; // was interrupt via encoder rotation called before last loop cycle?

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
    interruptedBySwitch = true;
}

#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
void interruptEncoder()
{
    interruptedByEncoder = true;
#ifdef POLL_ENCODER_INTERRUPTS
    encoder->tick();
#endif
}
#endif

void loopInput()
{
    bool inputProcessed = false; // was any input, or result of an input, or interrupt, processed this cycle?
    bool lastInterrupted = interruptedBySwitch || interruptedByEncoder;

    // check for interrupt since previous cycle
    if (lastInterrupted)
    {
        // yup, interrupt detected
        inputProcessed = true;
    }

// read rotary encoder switch
#ifdef USE_ENCODER_SWITCH_LOGIC
#ifdef ENCODER_SWITCH_LOGIC_POLL
    bool lastSwitch = encSwitch;              // preserve last switch state
    encSwitch = !digitalRead(PIN_ENC_SWITCH); // NC switch, invert
    if (lastSwitch != encSwitch)
    {
        // switch state toggled, do stuff...
        inputProcessed = true; // confirm input processed
        if (encSwitch)
        {
            // if switch is pressed, jump LED colour to opposite end of spectrum
            jumpLEDColor();
        }
    }
#endif
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
    if (interruptedBySwitch)
    {
        inputProcessed = true; // confirm input processed
        jumpLEDColor();
    }
#endif
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

        // add acceleration or delta multiplier, as needed
#ifdef USE_ENCODER_ACCELERATION
#ifdef POLL_ENCODER_INTERRUPTS
        unsigned long ms = encoder->getMillisBetweenRotations();
#else
        unsigned long ms = encoder.getMillisBetweenRotations();
#endif
        if (ms < longCutoff)
        {
            // do some acceleration using factors a and b
            // limit to maximum acceleration
            if (ms < shortCutoff)
            {
                ms = shortCutoff;
            }
            float ticksActual_float = a * ms + b;
            long deltaTicks = (long)ticksActual_float * (newPos - encPos);
            newPos = newPos + deltaTicks;
            // update encorder position to newPos
#ifdef POLL_ENCODER_INTERRUPTS
            encoder->setPosition(newPos);
#else
            encoder.setPosition(newPos);
#endif
            // done accounting for ms cutoff, calc delta
        }
        // position changed, read movement delta
        int delta = encPos - newPos; // encoder relative motion
#else
        // no acceleration, calc delta, check for multplier and proceed
        int delta = encPos - newPos; // encoder relative motion
#ifdef ENC_NONACCEL_MULTIPLIER
        delta *= ENC_NONACCEL_MULTIPLIER;
#endif
#endif

        // update LED colour by delta amount
        shiftLEDColor(delta);

        // confirm input processed
        inputProcessed = true;

        // update stored encoder position
        encPos = newPos;
    }

    if (inputProcessed)
    {
        // reset sleep timer
        resetSleepTimer();
    }

    // reset interrupted state
    if (lastInterrupted)
    {
        interruptedBySwitch = false;
        interruptedByEncoder = false;
    }
}

void inputSwitch()
{
}
void inputEncoder()
{
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