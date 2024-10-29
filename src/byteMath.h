#ifndef BYTEMATH_H
#define BYTEMATH_H

#include <Arduino.h>

// Subtracts byte `subtract` from given byte `value`, returning the result, WITHOUT UNDERFLOW. 
// Capped to `minValue`-255, default 'minValue = 0'
byte subtractByte(byte value, byte subtract, byte minValue = 0);
// Subtracts int `subtract` from given byte `value`, returning the result, WITHOUT OVER/UNDERFLOW. 
// Capped to `minValue`-255, default 'minValue = 0'
byte subtractByte(byte value, int subtract, byte minValue = 0);
// Adds byte `subtract` to given byte `value`, returning the result, WITHOUT OVERFLOW. 
// Capped to 0-`maxValue`, default `maxValue = 255`
byte addByte(byte value, byte add, byte maxValue = 255);
// Adds int `subtract` to given byte `value`, returning the result, WITHOUT OVER/UNDERFLOW.
// Capped to 0-`maxValue`, default `maxValue = 255`
byte addByte(byte value, int add, byte maxValue = 255);
// Returns a byte given float `lerp` (`0.0` to `1.0`) on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
byte lerpByte(float lerp, byte low = 0, byte high = 255);
// Returns a byte given float `lerp` (`0.0` to `1.0`) on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
// where the input `lerp` is raised to the power of `power` (default `3), eg 3 = cubic curve (see https://easings.net/#easeInCubic)
byte curvedLerpByte(float lerp, byte low = 0, byte high = 255, byte power = 3);

#endif // BYTEMATH_H
