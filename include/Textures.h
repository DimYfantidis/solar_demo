#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>

#include "CustomTypes.h"



// bmp file header 
typedef struct tagBitmapFileHeader {

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
typedef struct tagBitmapInfoHeader {
    
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


ubyte* loadBitmapToRGBArray(const char* filename, unsigned int* width, unsigned int* height)
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

    ubyte* image = (ubyte *)malloc((*width) * (*height) * 3 * sizeof(ubyte));

    // Reading the pixels of input image
    for (y = 0; y < *height; y++)
    {
        for (x = 0; x < *width; x++)
        {
            image[3 * (*width) * y + 3 * x + 2] = fgetc(fp);
            image[3 * (*width) * y + 3 * x + 1] = fgetc(fp);
            image[3 * (*width) * y + 3 * x + 0] = fgetc(fp);
        }
    }
    
	// printf("Header Info\n");
	// printf("--------------------\n");
	// printf("Size:%i\n", bmfh.bfSize);
	// printf("Offset:%i\n", bmfh.bfOffBits);
	// printf("--------------------\n");
	// printf("Size:%i\n", bmih.biSize);
	// printf("biWidth:%i\n", bmih.biWidth);
	// printf("biHeight:%i\n", bmih.biHeight);
	// printf("biPlanes:%i\n", bmih.biPlanes);
	// printf("biBitCount:%i\n", bmih.biBitCount);
	// printf("biCompression:%i\n", bmih.biCompression);
	// printf("biSizeImage:%i\n", bmih.biSizeImage);
	// printf("biXPelsPerMeter:%i\n", bmih.biXPelsPerMeter);
	// printf("biYPelsPerMeter:%i\n", bmih.biYPelsPerMeter);
	// printf("biClrUsed:%i\n", bmih.biClrUsed);
	// printf("biClrImportant:%i\n", bmih.biClrImportant);
	// printf("--------------------\n");

    fclose(fp);

    return image;
}


bool registerTexture(const char* filename, GLuint* textureID)
{
    unsigned int width;
    unsigned int height;

    ubyte* image = loadBitmapToRGBArray(filename, &width, &height);

    // TODO: improve error handling
    if (image == NULL) 
    {
        *textureID = 0;
        return false;
    }

    // Enable 2D texturing.
    glEnable(GL_TEXTURE_2D);
    // Generate a texture ID.
    glGenTextures(1, textureID); // Generate a texture ID
    // Bind the texture.
    glBindTexture(GL_TEXTURE_2D, *textureID);
    // Upload the texture data.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    // Set texture filtering to linear.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     // Set texture wrapping to repeat.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Bind the texture.
    glBindTexture(GL_TEXTURE_2D, *textureID);
    // Deallocated the image after upload.
    free(image);

    return true;
}

#endif TEXTURES_H