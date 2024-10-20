#include "input.h"
#include <EnableInterrupt.h>

static int encPos = 0; // current position of rotary encoder

volatile bool interruptedBySwitch = false;  // was interrupt via enc switch called before last loop cylce?
volatile bool interruptedByEncoder = false; // was interrupt via encoder rotation called before last loop cycle?

#ifdef POLL_ENCODER_INTERRUPTS
RotaryEncoder *encoder = nullptr;
#else
RotaryEncoder encoder(PIN_ENC_CLK, PIN_ENC_DAT, ENC_LATCH_MODE);
#endif

#ifdef USE_ENCODER_SWITCH_LOGIC
bool encSwitchInputProcessed = false; // was an input processed by all valid sources? flag to prevent multiple firings per input
#ifdef ENCODER_SWITCH_LOGIC_POLL
bool encSwitchPoll = false;  // is rotary encoder switch currently pressed, as determined by pin polling?
int encSwitchPollBuffer = 0; // raw buffer for reading enc switch over several frames following a valid pin poll
#endif
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
bool encSwitchInterrupt = false;  // is rotary encoder switch currently pressed, as determined by pin interrupt?
int encSwitchInterruptBuffer = 0; // raw buffer for reading enc switch for several frames following an interrupt
#endif
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
    bool switchInputReceived = false; // did we fully detect a switch input received by all relevant sources?
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
    // read switch interrupt buffer (read it up here because it gets referenced in multiple places below)
    if (interruptedBySwitch)
    {
        encSwitchInterruptBuffer = ENCODER_SWITCH_INPUT_BUFFER;
    }
    encSwitchInterrupt = encSwitchInterruptBuffer > 0;
#endif
#ifdef ENCODER_SWITCH_LOGIC_POLL
    bool lastSwitchPoll = encSwitchPoll; // preserve last switch state
    if (!digitalRead(PIN_ENC_SWITCH))    // NC switch, invert
    {
        encSwitchPollBuffer = ENCODER_SWITCH_INPUT_BUFFER;
    }
    encSwitchPoll = encSwitchPollBuffer > 0;
    if (lastSwitchPoll != encSwitchPoll)
    {
        // switch state toggled, do stuff...
        inputProcessed = true; // confirm input processed
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
        // both poll and interrupt are defined
    }
    if (encSwitchInterrupt && encSwitchPoll)
    {
        // both interrupted, AND encSwitchPoll is held
        switchInputReceived = true;
    }
    // end combined INTERRUPT + POLL logic
#else
        // poll logic only, no switch interrupt
        if (encSwitchPoll)
        {
            // if switch is pressed, confirm we've received input
            switchInputReceived = true;
        }
    } // end ENCODER_SWITCH_LOGIC_POLL only
#endif
#else // end ENCODER_SWITCH_LOGIC_POLL (and combined POLL + INTERRUPT logic)
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
    // interrupt logic only, no switch pin polling
    if (encSwitchInterrupt)
    {
        inputProcessed = true; // confirm input processed
        jumpLEDColor();
    }
#else
#error "neither switch interrupt nor pin polling logic defined, can't read switch input, at least one should be defined, otherwise undefine USE_ENCODER_SWITCH_LOGIC"
#endif
#endif
    // check to see if we're waiting for an input to process, or if we've already received one and are waiting for buffers to clear
    if (!encSwitchInputProcessed)
    {
        // input has NOT YET been processed, check if valid switch input received
        if (switchInputReceived)
        {
            // jump LED colour to opposite end of spectrum
            jumpLEDColor();
            // confirm we've processed the input
            encSwitchInputProcessed = true;
        }
    }
    else
    {
// input HAS been processed, check to see if the buffers are clear and we can reset the input
#if defined(ENCODER_SWITCH_LOGIC_POLL) && defined(ENCODER_SWITCH_LOGIC_INTERRUPT)
        // both poll and interrupt defined
        if (!switchInputReceived && !encSwitchInterrupt && !encSwitchPoll)
        {
            encSwitchInputProcessed = false; // all inputs and buffers cleared, reset input process
        }
#elif defined(ENCODER_SWITCH_LOGIC_POLL)
        // poll only
        if (!switchInputReceived && !encSwitchPoll)
        {
            encSwitchInputProcessed = false; // all inputs and buffers cleared, reset input process
        }
#elif defined(ENCODER_SWITCH_LOGIC_INTERRUPT)
        // interrupt only
        if (!switchInputReceived && !encSwitchInterrupt)
        {
            encSwitchInputProcessed = false; // all inputs and buffers cleared, reset input process
        }
#else
#error "neither switch interrupt nor pin polling logic defined, can't reset input processed, at least one should be defined, otherwise undefine USE_ENCODER_SWITCH_LOGIC"
#endif // end resetting enc switch input processing
    }
    // lastly, decrement input buffers
#ifdef ENCODER_SWITCH_LOGIC_POLL
    if (encSwitchPollBuffer > 0)
    {
        encSwitchPollBuffer -= DELAY_INTERVAL;
        if (encSwitchPollBuffer < 0)
        {
            encSwitchPollBuffer = 0;
        }
    }
#endif
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
    if (encSwitchInterruptBuffer > 0)
    {
        encSwitchInterruptBuffer -= DELAY_INTERVAL;
        if (encSwitchInterruptBuffer < 0)
        {
            encSwitchInterruptBuffer = 0;
        }
    }
#endif

#endif // end USE_ENCODER_SWITCH_LOGIC

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