#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>

#include "CustomTypes.h"
#include "BitmapImages.h"


bool registerTexture(const char* filename, GLuint* textureID)
{
    unsigned int width;
    unsigned int height;

    ubyte* image = loadBitmapToRGBArray(filename, &width, &height, false);

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

#endif // TEXTURES_H