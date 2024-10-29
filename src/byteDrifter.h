#ifndef BYTEDRIFTER_H
#define BYTEDRIFTER_H

#include <Arduino.h>
#include <Random16.h>
#ifndef Random16_h
#error Library "Random16" by AlexGyver required, search PIO libraries or visit https://github.com/GyverLibs/Random16
#endif

// value: desired target value
// vSpeed: how fast the actual value moves toward the target value
// vInterval: how many tick()s between value properties being recalculated
// decay: amount subtracted from value after each call of "iteration"
// dMod: amount subtracted from decay value after each "iteration"
// dDivisor: amount decay and dMod are divided by (rounded) before using
// dSpeed: speed that actual decay moves toward target decay
// dInterval: how many tick()s between decay properties being recalculated

// NOTE: default values, specifically speed and interval,
// are defined assuming a 30fps refresh rate, eg one call every ~33ms

class ByteDrifter
{
public:
    ByteDrifter(
        byte valueMin, byte valueMax,
        byte vSpeedMin, byte vSpeedMax,
        byte vIntervalMin, byte vIntervalMax,
        byte decayMin, byte decayMax,
        int8_t dModMin, int8_t dModMax,
        byte dDivisor,
        byte dSpeedMin, byte dSpeedMax,
        byte dIntervalMin, byte dIntervalMax,
        Random16 &rng) : _dDivisor(dDivisor),
                         _valueMin(valueMin),
                         _vSpeedMin(vSpeedMin),
                         _vIntervalMin(vIntervalMin),
                         _decayMin(decayMin),
                         _dModMin(dModMin),
                         _dSpeedMin(dSpeedMin),
                         _dIntervalMin(dIntervalMin),
                         _valueMax(valueMax),
                         _vSpeedMax(vSpeedMax),
                         _vIntervalMax(vIntervalMax),
                         _decayMax(decayMax),
                         _dModMax(dModMax),
                         _dSpeedMax(dSpeedMax),
                         _dIntervalMax(dIntervalMax),
                         rngPtr(rng)
    {
        // main constructor
        _iteration = 0;
    }
    ByteDrifter(Random16 &rng) : ByteDrifter(
                                     192, 255, // value min/max defaults
                                     4, 20,    // vSpeed min/max defaults
                                     4, 60,    // vInterval min/max defaults
                                     30, 240,  // decay min/max defaults
                                     -9, 25,   // dMod min/max defaults (signed, -128 ~ 127)
                                     10,       // dDivisor default
                                     25, 250,  // dSpeed min/max defaults
                                     5, 45,    // dInterval min/max defaults
                                     rng)
    {
        // delegate, no code here, see main constructor above
    }

    // Returns the current `value`, including iteration steps
    //
    // Optionally autoiterates in prep for next `value` (default `true`)
    byte getValue(bool autoIterate = true)
    {
        if (!autoIterate)
        {
            return _iteratedValue;
        }
        byte v = _iteratedValue;
        iterate();
        return v;
    }

    // iterate value for the next step
    void iterate()
    {
        _iteration++;
    }

    // Process one tick (move `value`/`decay`/`dMod` toward targets)
    //
    // Optionally reset iteration steps too (default `true`)
    void tick(bool autoResetIteration = true)
    {
        // value interval
        if (_vInterval == 0)
        {
            // value interval is zero, reset value params
            resetValueParams();
        }
        else
        {
            _vInterval--;
        }
        //
        // done, check for auto-reset iteration
        if (autoResetIteration)
        {
            resetIteration();
        }
    }

    void resetIteration()
    {
        _iteration = 0;
        _iteratedValue = _value;
        _iteratedDecay = _decay;
        _iteratedDMod = _dMod;
    }

    void resetValueParams()
    {
    }
    void resetDecayParams()
    {
    }
    void resetAllParams()
    {
        resetValueParams();
        resetDecayParams();
    }
    void resetAll()
    {
        resetAllParams();
        resetIteration();
    }

private:
    byte _value; // target value
    byte _vSpeed;
    byte _vInterval;
    byte _decay;  // target decay
    int8_t _dMod; // target dMod
    byte _dDivisor;
    byte _dSpeed;
    byte _dInterval;

    byte _actualValue; // actual value 
    byte _actualDecay; // actual decay 
    int8_t _actualDMod; // actual dMod

    byte _iteratedValue;
    byte _iteratedDecay;
    int8_t _iteratedDMod;

    byte _valueMin;
    byte _vSpeedMin;
    byte _vIntervalMin;
    byte _decayMin;
    int8_t _dModMin;
    byte _dSpeedMin;
    byte _dIntervalMin;

    byte _valueMax;
    byte _vSpeedMax;
    byte _vIntervalMax;
    byte _decayMax;
    int8_t _dModMax;
    byte _dSpeedMax;
    byte _dIntervalMax;

    Random16 &rngPtr;

    byte _iteration;
};

#endif // BYTEDRIFTER_H
