#ifndef TEXT_RENDERING_H
#define TEXT_RENDERING_H

#include <GL/glut.h>
#include <GL/freeglut.h>

#include "CustomTypes.h"


float windowMatrix[16];


void renderStringOnScreen(
    float x, float y, 
    void* font, const char* string, 
    ubyte r, ubyte g, ubyte b
)
{
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        {
            glLoadMatrixf(windowMatrix);
            // glDisable(GL_LIGHTING);

            glColor4f(r, g, b, 1.0f);
            glRasterPos2f(x, y);

            glutBitmapString(font, (const unsigned char*)string);
            //glEnable(GL_LIGHTING);
        }
        glPopMatrix();
    }
    glPopAttrib();
}

void renderStringInWorld(
    float x, float y, float z, 
    void* font, const char* string, 
    byte r, byte g, byte b
)
{
    glColor3ub(r, g, b);
    glRasterPos3f(x, y, z);
    for (int i = 0; string[i] != '\0'; ++i)
        glutBitmapCharacter(font, string[i]);
}


#endif // TEXT_RENDERING_H