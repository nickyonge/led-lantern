#ifndef BYTEDRIFTER_H
#define BYTEDRIFTER_H

#include <Arduino.h>
#include <Random16.h>
#ifndef Random16_h
#error Library "Random16" by AlexGyver required, search PIO libraries or visit https://github.com/GyverLibs/Random16
#endif
#include <byteMath.h>
#ifndef BYTEMATH_H
#error Need to import the ByteMath script by Duck Pond Studio / Nick Yonge
#endif

// #define DECAY_ENABLED // use decay to make value smaller with each iteration

#define VALUE_MIN 64
#define VALUE_MAX 255
#define VCURVEPOW 3
#define VSPEED_MIN 2
#define VSPEED_MAX 12
#define VINTERVAL_MIN 4
#define VINTERVAL_MAX 60
#ifdef DECAY_ENABLED
#define DECAY_MIN 20
#define DECAY_MAX 150
#define DCURVEPOW 3
#define DMOD_MIN -15
#define DMOD_MAX 25
#define DDIVISOR 6
#define DSPEED_MIN 25
#define DSPEED_MAX 250
#define DINTERVAL_MIN 5
#define DINTERVAL_MAX 45
#endif

//  192, 255, // value min/max defaults
//  4, 20,    // vSpeed min/max defaults
//  4, 60,    // vInterval min/max defaults
//  20, 150,  // decay min/max defaults
//  -15, 25,  // dMod min/max defaults (signed, -128 ~ 127)
//  6,       // dDivisor default (divide decay by this before value iteration)
//  25, 250,  // dSpeed min/max defaults
//  5, 45,    // dInterval min/max defaults

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
#ifdef DECAY_ENABLED
        byte decayMin, byte decayMax,
        int8_t dModMin, int8_t dModMax,
        byte dDivisor,
        byte dSpeedMin, byte dSpeedMax,
        byte dIntervalMin, byte dIntervalMax,
#endif
        Random16 &rng) :
#ifdef DECAY_ENABLED
                         _dDivisor(dDivisor),
                         _dDivisorHalf(dDivisor / 2),
#endif
                         _valueMin(valueMin),
                         _vSpeedMin(vSpeedMin),
                         _vIntervalMin(vIntervalMin),
#ifdef DECAY_ENABLED
                         _decayMin(decayMin),
                         _dModMin(dModMin),
                         _dSpeedMin(dSpeedMin),
                         _dIntervalMin(dIntervalMin),
#endif
                         _valueMax(valueMax),
                         _vSpeedMax(vSpeedMax),
                         _vIntervalMax(vIntervalMax),
#ifdef DECAY_ENABLED
                         _decayMax(decayMax),
                         _dModMax(dModMax),
                         _dSpeedMax(dSpeedMax),
                         _dIntervalMax(dIntervalMax),
#endif
                         _rng(rng)
    {
        // main constructor
        initialize();
    }
    ByteDrifter(Random16 &rng)
        : ByteDrifter(
              VALUE_MIN, VALUE_MAX,         // value min/max defaults
              VSPEED_MIN, VSPEED_MAX,       // vSpeed min/max defaults
              VINTERVAL_MIN, VINTERVAL_MAX, // vInterval min/max defaults
#ifdef DECAY_ENABLED
              DECAY_MIN, DECAY_MAX,         // decay min/max defaults
              DMOD_MIN, DMOD_MAX,           // dMod min/max defaults (signed, -128 ~ 127)
              DDIVISOR,                     // dDivisor default (divide decay by this before value iteration)
              DSPEED_MIN, DSPEED_MAX,       // dSpeed min/max defaults
              DINTERVAL_MIN, DINTERVAL_MAX, // dInterval min/max defaults
#endif
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
#ifdef DECAY_ENABLED
        // apply iteration to value from decay
        if (_iteratedValue > 0)
        {
            // determine rounded decay amount
            byte roundedDecay = (_iteratedDecay + _dDivisorHalf) / _dDivisor; // add 1/2div to effectively "round to nearest unit"
            _iteratedValue = subtractByte(_iteratedValue, roundedDecay);
        }
        // apply iteration to decay from dMod
        if (_dMod != 0)
        {
            if (_dMod > 0)
            {
                _iteratedDecay = subtractByte(_iteratedDecay, _dMod);
            }
            else
            {
                _iteratedDecay = addByte(_iteratedDecay, abs(_dMod));
            }
        }
#endif
        // increment iteration count
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
            randomizeValue();
        }
        else
        {
            _vInterval--;
        }
        // value drifts to targets
        if (_actualValue < _value)
        {
            _actualValue = addByte(_actualValue, _vSpeed, _value);
        }
        else if (_actualValue > _value)
        {
            _actualValue = subtractByte(_actualValue, _vSpeed, _value);
        }
#ifdef DECAY_ENABLED
        // decay interval
        if (_dInterval == 0)
        {
            // decay interval is zero, reset decay params
            randomizeDecay();
        }
        else
        {
            _dInterval--;
        }
        // decay drifts to targets
        if (_actualDecay < _decay)
        {
            _actualDecay = addByte(_actualDecay, _dSpeed, _decay);
        }
        else if (_actualDecay > _decay)
        {
            _actualDecay = subtractByte(_actualDecay, _dSpeed, _decay);
        }
#endif
        // done, check for auto-reset iteration
        if (autoResetIteration)
        {
            resetIteration();
        }
    }
    // resets the iteration count and parameters to their actual, non-iterated values
    void resetIteration()
    {
        _iteration = 0;
        _iteratedValue = _actualValue;
#ifdef DECAY_ENABLED
        _iteratedDecay = _actualDecay;
#endif
    }

private:
    byte _value;     // target value
    byte _vSpeed;    // value speed
    byte _vInterval; // value interval
#if defined(VCURVEPOW) && VCURVEPOW > 0
    byte _vCurvePow; // value curve power
#endif
#ifdef DECAY_ENABLED
    byte _decay;        // target decay
#if defined(DCURVEPOW) && DCURVEPOW > 0
    byte _dCurvePow;
#endif
    int8_t _dMod;       // target dMod
    byte _dDivisor;     // decay divisor
    byte _dDivisorHalf; // half divisor
    byte _dSpeed;       // decay speed
    byte _dInterval;    // decay interval
#endif

    byte _actualValue; // actual value
#ifdef DECAY_ENABLED
    byte _actualDecay; // actual decay
#endif

    byte _iteratedValue; // actual value + iterations
#ifdef DECAY_ENABLED
    byte _iteratedDecay; // actual decay + iterations
#endif

    // minimum ranges for values
    byte _valueMin;
    byte _vSpeedMin;
    byte _vIntervalMin;
#ifdef DECAY_ENABLED
    byte _decayMin;
    int8_t _dModMin;
    byte _dSpeedMin;
    byte _dIntervalMin;
#endif

    // maximum ranges for values
    byte _valueMax;
    byte _vSpeedMax;
    byte _vIntervalMax;
#ifdef DECAY_ENABLED
    byte _decayMax;
    int8_t _dModMax;
    byte _dSpeedMax;
    byte _dIntervalMax;
#endif

    Random16 &_rng; // reference to Random16

    byte _iteration;

    void randomizeValue()
    {
        _value = _rng.get(_valueMin, _valueMax);
        _vSpeed = _rng.get(_vSpeedMin, _vSpeedMax);
        _vInterval = _rng.get(_vIntervalMin, _vIntervalMax);
    }
#ifdef DECAY_ENABLED
    void randomizeDecay()
    {
        _decay = _rng.get(_decayMin, _decayMax);
        _dMod = (int8_t)_rng.get((byte)_dModMin, (byte)_dModMax);
        _dSpeed = _rng.get(_dSpeedMin, _dSpeedMax);
        _dInterval = _rng.get(_dIntervalMin, _dIntervalMax);
    }
#endif

    void randomizeAll()
    {
        randomizeValue();
#ifdef DECAY_ENABLED
        randomizeDecay();
#endif
    }

    void resetTargets()
    {
        _actualValue = _iteratedValue = _value;
#ifdef DECAY_ENABLED
        _actualDecay = _iteratedDecay = _decay;
#endif
    }
    void resetAll()
    {
        resetTargets();
        resetIteration();
    }

    void initialize()
    {
        randomizeAll();
        resetAll();
    }
};

#endif // BYTEDRIFTER_H
