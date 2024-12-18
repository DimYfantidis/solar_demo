#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#include <math.h>


typedef unsigned char byte;

typedef float vector3f[3];
typedef byte vector3ub[3];


float vectorLength3fv(vector3f v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float clampf(float x, float lo, float hi)
{
    return (x < lo ? lo : (x > hi ? hi : x));
}

char* strBuild(const char* org)
{
    size_t strLength = strlen(org);

    char* copy  = (char *)malloc((strLength + 1) * sizeof(char));

    strcpy(copy, org);

    copy[strLength] = '\0';

    return copy;
}

float AUtoR(float au)
{
    return au * 2000;
}

float AUtoD(float au)
{
    return au * 200;
}

#endif // CUSTOM_TYPES_H
