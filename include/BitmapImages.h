#ifndef IMAGES_H
#define IMAGES_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#   define _CRT_SECURE_NO_WARNINGS
#endif 

#include <stdio.h>
#include <stdlib.h>

#include "CustomTypes.h"


// bmp file header 
struct tagBitmapFileHeader {

#ifdef _MSC_VER
    #pragma pack(push, 1)
#endif 

	unsigned short bfType;

	unsigned int bfSize;

	unsigned short bfReserved1;
	unsigned short bfReserved2;

	unsigned int bfOffBits;

#ifdef _MSC_VER
    #pragma pack(pop)
#endif 
}
#ifdef __GNUC__
    __attribute__((packed))
#endif
;

// bmp extra file header
struct tagBitmapInfoHeader {
    
#ifdef _MSC_VER
    #pragma pack(push, 1)
#endif 

	unsigned int biSize;

	int biWidth;
	int biHeight;

	unsigned short biPlanes;
	unsigned short biBitCount;

	unsigned int biCompression;
	unsigned int biSizeImage;

	int biXPelsPerMeter;
	int biYPelsPerMeter;

	unsigned int biClrUsed;
	unsigned int biClrImportant;

#ifdef _MSC_VER
    #pragma pack(pop)
#endif 

}
#ifdef __GNUC__
    __attribute__((packed))
#endif
;

typedef struct tagBitmapInfoHeader BitmapInfoHeader;
typedef struct tagBitmapFileHeader BitmapFileHeader;


ubyte_t* loadBitmapToRGBArray(const char* filename, unsigned int* width, unsigned int* height, bool debug)
{
    FILE* fp;
    unsigned int x, y;
    BitmapFileHeader bmfh;
    BitmapInfoHeader bmih;


    // Opening the file: using "rb" mode to read this *binary* file
    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        fprintf(stderr, "Error: Bitmap image file is unavailable; Inspect \"%s\".\n", filename);
        return NULL;
    }

    // Reading the file header and any following bitmap information...
    fread(&bmfh, sizeof(BitmapFileHeader), 1, fp);
    fread(&bmih, sizeof(BitmapInfoHeader), 1, fp);

    // Extract the width & height from bmp header info
    *width = bmih.biWidth;
    *height = bmih.biHeight;

    ubyte_t* image = (ubyte_t *)malloc((*width) * (*height) * 3 * sizeof(ubyte_t));

    // Reading the pixels of input image
    for (y = 0; y < *height; y++)
    {
        size_t rowOffset = 3 * (*width) * y; 

        for (x = 0; x < *width; x++)
        {
            register size_t offset = rowOffset + 3 * x;

            image[offset + 2] = fgetc(fp);
            image[offset + 1] = fgetc(fp);
            image[offset + 0] = fgetc(fp);
        }
    }
    
    fclose(fp);

    if (debug)
    {   
        printf("File: %s\n", filename);
        printf("Header Info\n");
        printf("--------------------\n");
        printf("Size:%i\n", bmfh.bfSize);
        printf("Offset:%i\n", bmfh.bfOffBits);
        printf("--------------------\n");
        printf("Size:%i\n", bmih.biSize);
        printf("biWidth:%i\n", bmih.biWidth);
        printf("biHeight:%i\n", bmih.biHeight);
        printf("biPlanes:%i\n", bmih.biPlanes);
        printf("biBitCount:%i\n", bmih.biBitCount);
        printf("biCompression:%i\n", bmih.biCompression);
        printf("biSizeImage:%i\n", bmih.biSizeImage);
        printf("biXPelsPerMeter:%i\n", bmih.biXPelsPerMeter);
        printf("biYPelsPerMeter:%i\n", bmih.biYPelsPerMeter);
        printf("biClrUsed:%i\n", bmih.biClrUsed);
        printf("biClrImportant:%i\n", bmih.biClrImportant);
        printf("--------------------\n");
    }

    return image;
}


#endif // IMAGES_H