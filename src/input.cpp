#include "input.h"

#ifdef ENABLE_INPUT

// disable inclusion of unused interrupt pins from EnableInterrupt.h (save space)
// we are using all external interrupts (8), and pin change interrupts 6 and 7
// all of these are on port B, so we disable port A
#define EI_NOTPORTA

// including EnableInterrupt.h in the header file causes compile errors and for the life of me I can't figure out why
#include <EnableInterrupt.h>

static byte _loopIntervalInput = 0; // timer to keep track of loop() intervals for this class 

static int encPos = 0; // current position of rotary encoder

volatile bool clearBuffersAndTimers = false; // clear buffers/timers on next cycle (to avoid directly modifying them in sleep/wake cycle)
volatile bool interruptedBySwitch = false;   // was interrupt via enc switch called before last loop cylce?
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
volatile bool interruptedByEncoder = false; // was interrupt via encoder rotation called before last loop cycle?
#endif

#ifdef POLL_ENCODER_INTERRUPTS
RotaryEncoder *encoder = nullptr;
#else
RotaryEncoder encoder(PIN_ENC_CLK, PIN_ENC_DAT, ENC_LATCH_MODE);
#endif

#ifdef USE_ENCODER_SWITCH_LOGIC
bool encSwitchInputProcessed = false; // was an input processed by all valid sources? flag to prevent multiple firings per input
int encSwitchInputDelay = 0;          // forced delay to prevent processing of any switch input
#ifdef ENCODER_SWITCH_LOGIC_POLL
bool encSwitchPoll = false;  // is rotary encoder switch currently pressed, as determined by pin polling?
int encSwitchPollBuffer = 0; // raw buffer for reading enc switch over several frames following a valid pin poll
#endif
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
int encSwitchInterruptBuffer = 0; // raw buffer for reading enc switch for several frames following an interrupt
#endif
#if (defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0) || (defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0)
int encSwitchHeldTime = 0; // if enc sleep timeout, OR hold switch for button, use `encSwitchHeldTime`
#if (defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0)
#if (defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0)
int encBrightnessDeltaBuffer = 0;               // tracked delta valueu for enc brightness adjustment, to see about disabling `goToSleep` hold timer
bool encSleepDisabledByBrightnessDelta = false; // has sleep timeout been disabled by the brightness delta?
#endif
#endif
#endif
#endif

#endif

void setupInput()
{
#ifdef ENABLE_INPUT
// create encoder if necessary
#ifdef POLL_ENCODER_INTERRUPTS
    encoder = new RotaryEncoder(PIN_ENC_CLK, PIN_ENC_DAT, ENC_LATCH_MODE);
#endif
    // encoder switch pin
    pinMode(PIN_ENC_SWITCH, INPUT_PULLUP);
    // enable interrupt on enc switch pin
    enableInterrupt(PIN_ENC_SWITCH | PINCHANGEINTERRUPT, interruptSwitch, FALLING);
    // check for encoder data interrupts
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
    enableInterrupt(PIN_ENC_DAT, interruptEncoder, CHANGE);
    enableInterrupt(PIN_ENC_CLK, interruptEncoder, CHANGE);
#endif
#endif
}

void loopInput()
{
#ifdef ENABLE_INPUT
// check input loop delay
#if defined(LOOP_INTERVAL_INPUT) && LOOP_INTERVAL_INPUT > 1
    _loopIntervalInput += DELAY_INTERVAL;
    if (_loopIntervalInput >= LOOP_INTERVAL_INPUT)
    {
        _loopIntervalInput -= LOOP_INTERVAL_INPUT;
    }
    else
    {
        return;
    }
#endif
    // check to clear buffers/timers from a wakeup cycle
    if (clearBuffersAndTimers)
    {
#ifdef USE_ENCODER_SWITCH_LOGIC
#ifdef ENCODER_SWITCH_LOGIC_POLL
        encSwitchPollBuffer = 0;
#if (defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0) || (defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0)
        encSwitchHeldTime = 0;
#if (defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0) && (defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0)
        encBrightnessDeltaBuffer = 0;
        encSleepDisabledByBrightnessDelta = false;
#endif
#endif
#endif
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
        encSwitchInterruptBuffer = 0;
#endif
        encSwitchInputDelay = ENCODER_SWITCH_WAKE_INPUT_DELAY;
#endif
    }

    // prep input state properties
    bool inputProcessed = false; // was ANY input/interrupt processed this cycle?
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
    bool lastInterrupted = interruptedBySwitch || interruptedByEncoder; // was an interrupt received by either switch or encoder?
#else
    bool lastInterrupted = interruptedBySwitch; // was an interrupt received by switch?
#endif

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
        if (encSwitchInputDelay == 0) // confirm input read delay is done
        {
            encSwitchInterruptBuffer = ENCODER_SWITCH_INPUT_BUFFER;
        }
    }
    bool encSwitchInterrupt = encSwitchInterruptBuffer > 0;
#endif
#ifdef ENCODER_SWITCH_LOGIC_POLL
    bool lastSwitchPoll = encSwitchPoll; // preserve last switch state
    if (encSwitchInputDelay == 0)        // confirm input read delay is done
    {
        if (!digitalRead(PIN_ENC_SWITCH)) // NC switch, invert
        {
            encSwitchPollBuffer = ENCODER_SWITCH_INPUT_BUFFER;
        }
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
#ifdef ENCODER_SWITCH_JUMPS_LEDS
            // jump LED colour to opposite end of spectrum
            jumpLEDColor();
#endif
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
    // lastly, decrement input buffers and delays
#ifdef ENCODER_SWITCH_LOGIC_POLL
    if (encSwitchPollBuffer > 0)
    {
        encSwitchPollBuffer -= LOOP_INTERVAL_INPUT;
        if (encSwitchPollBuffer < 0)
        {
            encSwitchPollBuffer = 0;
        }
    }
#endif
#ifdef ENCODER_SWITCH_LOGIC_INTERRUPT
    if (encSwitchInterruptBuffer > 0)
    {
        encSwitchInterruptBuffer -= LOOP_INTERVAL_INPUT;
        if (encSwitchInterruptBuffer < 0)
        {
            encSwitchInterruptBuffer = 0;
        }
    }
#endif
    if (encSwitchInputDelay > 0)
    {
        encSwitchInputDelay -= LOOP_INTERVAL_INPUT;
        if (encSwitchInputDelay < 0)
        {
            encSwitchInputDelay = 0;
        }
    }

// track how long encoder input has been held down (requires poll logic) if tracking either hold-to-sleep or hold-to-adj-brightness
#if (defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0) || (defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0)
    if (encSwitchPoll)
    {
        // holding switch down
        encSwitchHeldTime += LOOP_INTERVAL_INPUT;
#if defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0
        // check for sleep timeout
        if (encSwitchHeldTime >= ENC_HELD_SLEEP_TIMEOUT)
        {
// ensure sleep timer isn't disabled by brightness delta
#if defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0
            if (encSleepDisabledByBrightnessDelta)
            {
                // brightness delta held down long enough to disable sleep timeout, just lock the timer
                encSwitchHeldTime = ENC_HELD_SLEEP_TIMEOUT;
            }
            else
            {
                // not disabled, and switch held long enough to put device to sleep, nighty night
                goToSleep();
                // return;
            }
#else
            // no brightness check, and switch held long enough to put device to sleep, nighty night
            goToSleep();
            // return;
#endif
        }
#else
        // not tracking sleep, just brightness adjustment, lock encSwitchHeldTime to ENC_HELD_ADJUST_BRIGHTNESS
        if (encSwitchHeldTime > ENC_HELD_ADJUST_BRIGHTNESS)
        {
            encSwitchHeldTime = ENC_HELD_ADJUST_BRIGHTNESS;
        }
#endif // end encSwitchHeldTime checks
    }
    else
    {
        // switch not held, reset timer
        encSwitchHeldTime = 0;
#if (defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0) && (defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0)
        // brightness timer defined, ensurue brightness delta buffer also reset
        encBrightnessDeltaBuffer = 0;
        encSleepDisabledByBrightnessDelta = false;
#endif
    }
#endif

#endif // end USE_ENCODER_SWITCH_LOGIC

    // update and read rotary encoder movement (interrupted)
#ifdef POLL_ENCODER_INTERRUPTS
#ifdef POLL_ENCODER_LOOP
    encoder->tick();
#endif
    int newPos = encoder->getPosition();
#else
    // update and read rotary encoder movement (non-interrupted)
#ifdef POLL_ENCODER_LOOP
    encoder.tick();
#endif
    int newPos = encoder.getPosition();
#endif

    // check for position change
    if (encPos != newPos)
    {

        // add acceleration or delta multiplier, as needed
#ifdef ENC_ROTATION_ACCELERATION
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

        // check for delta
        if (delta != 0)
        {
            // update LED colour/brightness by delta amount
#if defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0
            if (encSwitchHeldTime >= ENC_HELD_ADJUST_BRIGHTNESS)
            {
                // enc switch is held, adjust brightness
                shiftLEDBrightness(delta);
#if (defined(ENC_HELD_ADJUST_BRIGHTNESS) && ENC_HELD_ADJUST_BRIGHTNESS > 0) && (defined(ENC_HELD_SLEEP_TIMEOUT) && ENC_HELD_SLEEP_TIMEOUT > 0)
                // track brightness delta to check for sleep timeout disable
                encBrightnessDeltaBuffer += delta;
                if (abs(encBrightnessDeltaBuffer) >= ENC_ADJUST_BRIGHTNESS_AMT_DISABLES_SLEEP)
                {
                    // yup, brightness has changed sufficiently to disable sleep timeout
                    encSleepDisabledByBrightnessDelta = true;
                }
#endif
            }
            else
            {
                // enc switch not held, adjust colour
                shiftLEDColor(delta);
            }
#else
            // update LED colour only
            shiftLEDColor(delta);
#endif
            // confirm input processed
            inputProcessed = true;
        }

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
#if defined(POLL_ENCODER_INTERRUPTS) || defined(ENC_ROTATION_WAKES_DEVICE)
        interruptedByEncoder = false;
#endif
    }
#endif // ENABLE_INPUT
}

#ifdef ENABLE_INPUT
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
#endif

void sleepInput()
{
#ifdef ENABLE_INPUT
    // reset switch timers and input buffers
    clearBuffersAndTimers = true;
// disable interrupts as needed
#ifndef ENC_SWITCH_WAKES_DEVICE
    disableInterrupt(PIN_ENC_SWITCH | PINCHANGEINTERRUPT);
#endif
#ifndef ENC_ROTATION_WAKES_DEVICE
#ifdef POLL_ENCODER_INTERRUPTS
    disableInterrupt(PIN_ENC_DAT);
    disableInterrupt(PIN_ENC_CLK);
#endif
#endif
#endif
}
void wakeInput()
{
#ifdef ENABLE_INPUT
// re-enable interrupts
#ifndef ENC_SWITCH_WAKES_DEVICE
    enableInterrupt(PIN_ENC_SWITCH | PINCHANGEINTERRUPT, interruptSwitch, FALLING);
#endif
#ifndef ENC_ROTATION_WAKES_DEVICE
#ifdef POLL_ENCODER_INTERRUPTS
    enableInterrupt(PIN_ENC_DAT, interruptEncoder, CHANGE);
    enableInterrupt(PIN_ENC_CLK, interruptEncoder, CHANGE);
#endif
#endif
    // ensure clear buffers is still true, post wakeup, for next cycle
    clearBuffersAndTimers = true;
#endif
}
bool validWakeUp()
{
#ifdef ENABLE_INPUT
// check valid wakeup types
#ifdef ENC_SWITCH_WAKES_DEVICE
    // at least, check for that
    if (interruptedBySwitch)
    {
        // check for invalid switch timing
        // TODO: invalid sw time
    }
#endif
#endif
    // the only invalid circumstance involves the switch, and we checked for that, return true
    return true;
}