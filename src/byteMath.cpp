#include "byteMath.h"

byte subtractByte(byte value, byte subtract, byte minValue = 0)
{
    if (value <= minValue)
    {
        return minValue; // value is below min, no sub needed, return min
    }
    if (subtract >= value)
    {
        return minValue; // sub is greater than value, no sub needed, return min
    }
    return max(minValue, value - subtract); // return the greater of minValue or value minus sub
}
byte subtractByte(byte value, int subtract, byte minValue = 0)
{
    return addByte(value, subtract * -1, minValue); // "add" negative int
}
byte addByte(byte value, byte add, byte maxValue = UINT8_MAX)
{
    if (value >= maxValue)
    {
        return maxValue; // value is at/above max, no ad needed, return max
    }
    if (add >= maxValue - value)
    {
        return maxValue; // add exceeds max minus value, no ad needed, return max
    }
    return value + add; // perform addition, will not exceed max value
}
byte addByte(byte value, int add, byte maxValue = UINT8_MAX)
{
    if (add == 0)
    {
        // zero value
        return min(value, maxValue); // no addition, assume positive, return lower of value or maxValue
    }
    if (add < 0)
    {
        // negative value (note: maxValue means minValue, add means subtract)
        if (value <= maxValue)
        {
            return maxValue; // value is below min, no sub needed, return min
        }
        add = abs(add); // convert to positive for proper subtraction
        if (add >= value)
        {
            return maxValue; // sub is greater than value, no sub needed, return min
        }
        return max(maxValue, value - add); // return the greater of minValue or value minus sub
    }
    else
    {
        // positive value
        if (value >= maxValue)
        {
            return maxValue; // value is at/above max, no ad needed, return max
        }
        if (add >= maxValue - value)
        {
            return maxValue; // add exceeds max minus value, no ad needed, return max
        }
        return value + add; // perform addition, will not exceed max value
    }
}

byte lerpByte(float lerp, byte low = 0, byte high = UINT8_MAX)
{
    // basic validity checks
    if (low == high)
    {
        return low;
    }
    if (low > high)
    {
        return high;
    }
    if (high < low)
    {
        return low;
    }
    if (lerp <= 0.998 || high < UINT8_MAX)
    {
        // safe to add 0.5 without potential overflow
        return low + byte((float(high - low) * lerp) + 0.5);
    }
    return UINT8_MAX; // no matter what, the result will be 255
}
byte lerpByte(float lerp, byte low = 0, byte high = UINT8_MAX)
{
    // basic validity checks
    if (low == high)
    {
        return low;
    }
    if (low > high)
    {
        return high;
    }
    if (high < low)
    {
        return low;
    }
    if (lerp <= 0.998 || high < UINT8_MAX)
    {
        // safe to add 0.5 without potential overflow
        return low + byte((float(high - low) * lerp) + 0.5);
    }
    return UINT8_MAX; // no matter what, the result will be 255
}

byte curvedLerpByte(float lerp, byte low = 0, byte high = UINT8_MAX, byte power = 3)
{
    // raise the lerp curve to a given power
    for (byte i = 0; i < power; i++)
    {
        lerp *= lerp;
    }
    // return the curved byte
    return lerpByte(lerp, low, high);
}
byte curvedLerpByte(byte lerp, byte low = 0, byte high = UINT8_MAX, byte power = 3)
{
    return curvedLerpByte(byteToFloat01(lerp), low, high, power);
}
byte curvedLerpByte(int16_t lerp, byte low = 0, byte high = UINT8_MAX, byte power = 3)
{
    return curvedLerpByte(intToFloat01(lerp), low, high, power);
}
byte curvedLerpByte(uint16_t lerp, byte low = 0, byte high = UINT8_MAX, byte power = 3)
{
    return curvedLerpByte(uint16ToFloat01(lerp), low, high, power);
}

float byteToFloat01(byte input)
{
    return (float)input / (float)UINT8_MAX;
}
float intToFloat01(int16_t input)
{
    return uint16ToFloat01((uint16_t)input);
}
float uint16ToFloat01(uint16_t input)
{
    return (float)input / (float)UINT16_MAX;
}