#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/freeglut.h>

#include "Camera.h"
#include "CustomTypes.h"
#include "TextRendering.h"
#include "KeyboardCallback.h"


struct MenuScreen
{
    const float* pWindowMatrix;

    int screenWidth;
    int screenHeight;

    int numOptions;

    char** optionNames;

    int currentlySelectedOption;

    float optionBoxWidth;
    float optionBoxHeight;
};
typedef struct MenuScreen MenuScreen;


MenuScreen* initMenuScreen(const float* pWindowMatrix, const int n_options, ...)
{
    MenuScreen* m = (MenuScreen *)malloc(sizeof(MenuScreen));

    m->pWindowMatrix = pWindowMatrix;

    m->numOptions = n_options;

    m->optionNames = (char **)malloc(n_options * sizeof(char *));

    m->currentlySelectedOption = 0;


    va_list optionStringsArgumentList;

    va_start(optionStringsArgumentList, n_options);

    for (int i = 0; i < n_options; ++i)
    {
        const char* option = va_arg(optionStringsArgumentList, const char*);

        m->optionNames[i] = strBuild(option);
    }

    va_end(optionStringsArgumentList);

    return m;
}

MenuScreen* initMenuScreenEmpty(float* pWindowMatrix, int n_options)
{
    MenuScreen* m = (MenuScreen *)malloc(sizeof(MenuScreen));

    m->pWindowMatrix = pWindowMatrix;
    
    m->numOptions = n_options;

    m->optionNames = (char **)malloc(n_options * sizeof(char *));
    
    m->currentlySelectedOption = 0;


    for (int i = 0; i < n_options; ++i) 
    {
        m->optionNames[i] = NULL;
    }

    return m;
}

MenuScreen* setMenuScreenDimensions(MenuScreen* m, int width, int height)
{
    m->screenWidth = width;
    m->screenHeight = height;
    return m;
}

MenuScreen* setMenuScreenBoxDimensions(MenuScreen* m, float width, float height)
{
    m->optionBoxWidth = width;
    m->optionBoxHeight = height;
    return m;
}

bool assignMenuScreenElement(MenuScreen* m, int idx, const char* option_at_idx)
{
    if (idx < 0 || idx > m->numOptions) {
        return false;
    }
    if (m->optionNames[idx] != NULL) {
        free(m->optionNames[idx]);
    }
    m->optionNames[idx] = strBuild(option_at_idx);

    return true;
}

void renderMenuScreen(MenuScreen* m)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(m->pWindowMatrix);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    glColor4f(.3f, .3f, .3f, 0.5f);


    float hiBorder = 0.5f + (m->numOptions / 2 + 1) * .05f;
    float loBorder = 0.5f - (m->numOptions / 2 + 1) * .05f;

    glBegin(GL_QUADS);
    {
        glColor4f(.3f, .3f, .3f, 0.5f);

        glVertex3f(0.0f * m->screenWidth, 1.0f * m->screenHeight, .0f);
        glVertex3f(1.0f * m->screenWidth, 1.0f * m->screenHeight, .0f);
        glVertex3f(1.0f * m->screenWidth, 0.0f * m->screenHeight, .0f);
        glVertex3f(0.0f * m->screenWidth, 0.0f * m->screenHeight, .0f);
        
        glColor4f(.1f, .1f, .1f, 1.0f);

        glVertex3f(0.40f * m->screenWidth, hiBorder * m->screenHeight, .1f);
        glVertex3f(0.60f * m->screenWidth, hiBorder * m->screenHeight, .1f);
        glVertex3f(0.60f * m->screenWidth, loBorder * m->screenHeight, .1f);
        glVertex3f(0.40f * m->screenWidth, loBorder * m->screenHeight, .1f);
    }
    glEnd();

    glTranslatef(.0f, .0f, .2f);

    float offset = hiBorder - .05f;

    for (int i = 0; i < m->numOptions; ++i)
    {
        glColor4f(0xFF, 0xFF, 0xFF, 1.0f);

        if (i == m->currentlySelectedOption) 
        {
            glRasterPos2f(0.46f * m->screenWidth, offset * m->screenHeight);
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '>');
        }

        glRasterPos2f(0.48f * m->screenWidth, offset * m->screenHeight);

        glutBitmapString(
            GLUT_BITMAP_9_BY_15, 
            (const unsigned char*)m->optionNames[i]
        );

        offset -= .05f;
    }
    

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

const char* menuScreenHandler(MenuScreen* m, int* optionID)
{
    if (arrowDownLoaded)
    {
        if (m->currentlySelectedOption < m->numOptions - 1)
            m->currentlySelectedOption += 1;
        arrowDownLoaded = false;
    }
    if (arrowUpLoaded)
    {
        if (m->currentlySelectedOption > 0)
            m->currentlySelectedOption -= 1;
        arrowUpLoaded = false;
    }
    
    if (keystrokes['\n'] || keystrokes['\f'] || keystrokes['\r'])
    {
        if (optionID != NULL)
            *optionID = m->currentlySelectedOption;
        
        return m->optionNames[m->currentlySelectedOption];
    }
    
    return NULL;
}

void deleteMenuScreen(MenuScreen* m)
{
    for (int i = 0; i < m->numOptions; ++i) 
    {
        if (m->optionNames[i] != NULL)
            free(m->optionNames[i]);
    }
    free(m->optionNames);
    free(m);
}

#endif // MENU_SCREEN_H