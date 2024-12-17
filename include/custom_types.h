#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#include <math.h>


typedef float vector3f[3];

float vectorLength3fv(vector3f v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float clampf(float x, float lo, float hi)
{
    return (x < lo ? lo : (x > hi ? hi : x));
}

#endif // CUSTOM_TYPES_H
