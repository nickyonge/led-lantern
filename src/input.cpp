#include "input.h"

RotaryEncoder encoder(PIN_ENC_CLK, PIN_ENC_DAT, RotaryEncoder::LatchMode::FOUR3);

void setupInput()
{
    // encoder switch pin
    pinMode(PIN_ENC_SWITCH, INPUT_PULLUP);
    // enable interrupt on enc switch pin
    enableInterrupt(PIN_ENC_SWITCH, onInterrupt, FALLING);
}

void onInterrupt()
{
    lastInterrupted = true;
}

void loopInput()
{
    // check for interrupt since previous cycle  
    if (lastInterrupted) {
        // yup, interrupt detected
        lastInterrupted = false;
    }

    // read rotary encoder switch
    bool lastSwitch = encSwitch;
    encSwitch = !digitalRead(PIN_ENC_SWITCH); // NC switch, invert

    // update rotary encoder movement
    encoder.tick();

    int newPos = encoder.getPosition(); // read movement
    // check for position change
    if (encPos != newPos)
    {
        // position changed, read movement delta 
        int delta = encPos - newPos;
        // update LED colour by delta amount 

        shiftLEDColor(delta);
        encPos = newPos;
    }
}