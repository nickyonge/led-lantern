#ifndef BYTEMATH_H
#define BYTEMATH_H

#include <Arduino.h>

#define DEFAULT_CURVE_LERP_POWER 3 // default power to use in `curvedLerpByte`

// --- Basic add / subtract byte math with overflow prevention

// Subtracts byte `subtract` from given byte `value`, returning the result, WITHOUT UNDERFLOW.
// Capped to `minValue`-255, default 'minValue = 0'
byte subtractByte(byte value, byte subtract, byte minValue = 0);
// Subtracts int `subtract` from given byte `value`, returning the result, WITHOUT OVER/UNDERFLOW.
// Capped to `minValue`-255, default 'minValue = 0'
byte subtractByte(byte value, int subtract, byte minValue = 0);
// Adds byte `subtract` to given byte `value`, returning the result, WITHOUT OVERFLOW.
// Capped to 0-`maxValue`, default `maxValue = 255`
byte addByte(byte value, byte add, byte maxValue = UINT8_MAX);
// Adds int `subtract` to given byte `value`, returning the result, WITHOUT OVER/UNDERFLOW.
// Capped to 0-`maxValue`, default `maxValue = 255`
byte addByte(byte value, int add, byte maxValue = UINT8_MAX);

// --- Convert byte and int values to 0.0-1.0 normalized floats
//     (I know, technically it's some non-byte math, oops ^_^ )

// Convert the given byte `input` to a float from `0.0`-`1.0`,
// where `0.0` corresponds to byte `0` and `1.0` is byte `255` (or `UINT8_MAX`)
float byteToFloat01(byte input);
// Convert the given int `input` to a float from `0.0`-`1.0`,
// where `0.0` corresponds to int `-32768` and `1.0` is int `32767` (or `INT16_MAX`)
float intToFloat01(int16_t input);
// Convert the given unsigned int `input` to a float from `0.0`-`1.0`,
// where `0.0` corresponds to int `0` and `1.0` is int `65535` (or `UINT16_MAX`)
float uint16ToFloat01(uint16_t input);

// --- Lerp byte, and lerp along a curve, using 0.0-1.0 normalized floats

// Returns a byte, given float `lerp` (`0.0` to `1.0`) on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
byte lerpByte(float lerp, byte low = 0, byte high = UINT8_MAX);
// Returns a byte, given float `lerp` (`0.0` to `1.0`) on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
// where the input `lerp` is raised to the power of `power` (default `3), eg 3 = cubic curve (see https://easings.net/#easeInCubic)
byte curvedLerpByte(float lerp, byte low = 0, byte high = UINT8_MAX, byte power = DEFAULT_CURVE_LERP_POWER);
// Returns a byte, given byte `lerp` (`0` to `255`, which is normalized to a float from `0.0` to `1.0`),
// on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
// where the input `lerp` is raised to the power of `power` (default `3), eg 3 = cubic curve (see https://easings.net/#easeInCubic)
byte curvedLerpByte(byte lerp, byte low = 0, byte high = UINT8_MAX, byte power = DEFAULT_CURVE_LERP_POWER);
// Returns a byte, given int `lerp` (`-32768` to `32767`, which is normalized to a float from `0.0` to `1.0`),
// on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
// where the input `lerp` is raised to the power of `power` (default `3), eg 3 = cubic curve (see https://easings.net/#easeInCubic)
byte curvedLerpByte(int16_t lerp, byte low = 0, byte high = UINT8_MAX, byte power = DEFAULT_CURVE_LERP_POWER);
// Returns a byte, given unsigned int `lerp` (`0` to `65535`, which is normalized to a float from `0.0` to `1.0`),
// on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
// where the input `lerp` is raised to the power of `power` (default `3), eg 3 = cubic curve (see https://easings.net/#easeInCubic)
byte curvedLerpByte(uint16_t lerp, byte low = 0, byte high = UINT8_MAX, byte power = DEFAULT_CURVE_LERP_POWER);

#endif // BYTEMATH_H
