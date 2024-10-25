#include "bytemath.h"

byte subtractByte(byte value, byte subtract)
{
    if (subtract < value)
    {
        return value - subtract;
    }
    return 0;
}
byte subtractByte(byte value, int subtract)
{
    return addByte(value, subtract * -1);
}
byte addByte(byte value, byte add)
{
    if (add >= 255 - value)
    {
        return 255;
    }
    return value + add;
}
byte addByte(byte value, int add)
{
    if (add < 0)
    {
        // negative value
        if (add + value > 0)
        {
            return add + value;
        }
        return 0;
    }
    else
    {
        // positive or zero value
        if (add >= 255 - value)
        {
            return 255;
        }
        return value + add;
    }
}