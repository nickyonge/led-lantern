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
byte addByte(byte value, byte add, byte maxValue = 255)
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
byte addByte(byte value, int add, byte maxValue = 255)
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

// Returns a byte given float `lerp` (`0.0` to `1.0`) on a range from `low` (`0.0`, default `0`) to `high` (`1.0`, default `255`)
// byte lerpByte(float lerp, byte low = 0, byte high = 255);

byte lerpByte(float lerp, byte low = 0, byte high = 255)
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
    if (lerp <= 0.998 || high < 255)
    {
        // safe to add 0.5 without potential overflow
        return low + byte((float(high - low) * lerp) + 0.5);
    }
    return 255; // no matter what, the result will be 255
}

byte curvedLerpByte(float lerp, byte low = 0, byte high = 255, byte power = 3)
{
    // raise the lerp curve to a given power
    for (byte i = 0; i < power; i++)
    {
        lerp *= lerp;
    }
    // return the curved byte
    return lerpByte(lerp, low, high);
}