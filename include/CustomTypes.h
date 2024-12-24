#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif 

#include <math.h>
#include <stdarg.h>


typedef char byte;
typedef unsigned char ubyte;

typedef float vector3f[3];
typedef ubyte vector3ub[3];


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

// Concatenates a total of n strings, each one succeeding the other.
char* strCat(int n_strings, ...)
{
    va_list stringsToBeConcatenatedList;

    va_start(stringsToBeConcatenatedList, n_strings);

    size_t lengthOfResultString = 0;

    for (int i = 0; i < n_strings; ++i)
    {
        const char* arg = va_arg(stringsToBeConcatenatedList, const char*);

        lengthOfResultString += strlen(arg);
    }

    // Restart the VA List in order to re-iterate it.
    va_end(stringsToBeConcatenatedList);
    va_start(stringsToBeConcatenatedList, n_strings);
    

    char* result = (char *)malloc((lengthOfResultString + 1) * sizeof(char));

    int k = 0;

    for (int i = 0; i < n_strings; ++i)
    {
        const char* arg = va_arg(stringsToBeConcatenatedList, const char*);

        for (int j = 0; arg[j] != '\0'; ++j)
        {
            result[k++] = arg[j];
        }
    }

    va_end(stringsToBeConcatenatedList);

    result[lengthOfResultString] = '\0';

    return result;
}

float AUtoR(float au)
{
    return au * 450;
}

size_t getFileSizeInBytes(const char* filename)
{
    FILE * fp;
    fp = fopen(filename, "rb");

    if (fp == NULL) 
        return 0;

    fseek(fp, 0L, SEEK_END);

    size_t size = (size_t)ftell(fp);

    rewind(fp);

    fclose(fp);

    return size;
}


#endif // CUSTOM_TYPES_H
