#ifndef BYTEMATH_H
#define BYTEMATH_H

#include <Arduino.h>

// Subtracts byte `subtract` from given byte `value`, returning the result, WITHOUT OVERFLOW. Capped to 0-255
byte subtractByte(byte value, byte subtract);
// Subtracts int `subtract` from given byte `value`, returning the result, WITHOUT OVERFLOW. Capped to 0-255
byte subtractByte(byte value, int subtract);
// Adds byte `subtract` to given byte `value`, returning the result, WITHOUT OVERFLOW. Capped to 0-255
byte addByte(byte value, byte add);
// Adds int `subtract` to given byte `value`, returning the result, WITHOUT OVERFLOW. Capped to 0-255
byte addByte(byte value, int add);

#endif // BYTEMATH_H