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

#endif // BYTEMATH_H
