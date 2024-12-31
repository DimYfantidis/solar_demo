#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#   define _USE_MATH_DEFINES
#endif 

#include <math.h>
#include <stdarg.h>


typedef char byte_t;

typedef unsigned char ubyte_t;

typedef double real_t;

typedef float vector4f[4];
typedef float vector3f[3];
typedef float vector2f[2];

typedef double vector4d[4];
typedef double vector3d[3];
typedef double vector2d[2];

typedef real_t vector4r[4];
typedef real_t vector3r[3];
typedef real_t vector2r[2];

typedef ubyte_t vector3ub[3];


float vectorLength3fv(vector3f v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

real_t vectorLength3rv(vector3r v)
{
    return (real_t)sqrt((double)(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
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

real_t AUtoR(real_t au)
{
    return au * 200.0;
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

/* Performs matrix multiplication. It is considered that the input and output matrices
 * are allocated on the stack as float[][] and not as float**. Thus they must be flattened
 * by casting them to float* before passing them as arguments.
 * Input:
 *	- A, B: The matrices who will be multplied by A*B
 *	- N, K, M: The dimensions of the matrices, A:NxK & B:KxM
 * Output:
 *	- prod: The product of A and B
 */
void matrixMultiplication(real_t* prod, const real_t* A, const real_t* B, size_t N, size_t K, size_t M)
{
	size_t i, j, k;

	for (i = 0; i < N; ++i)
	{
		for (j = 0; j < M; ++j)
		{
			prod[M * i + j] = .0f;
			for (k = 0; k < K; ++k)
			{
				prod[M * i + j] += A[K * i + k] * B[M * k + j];
			}
		}
	}
}



#endif // CUSTOM_TYPES_H
