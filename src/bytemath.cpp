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