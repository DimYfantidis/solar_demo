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


typedef struct MenuScreen
{
    const float* pWindowMatrix;

    int screenWidth;
    int screenHeight;

    int numOptions;

    char* title;

    char** optionNames;

    int charLenTitle;

    int currentlySelectedOptionIndex;

} MenuScreen;


// Menu constructor #1 (heap-allocated).
MenuScreen* initMenuScreen(const char* title, const float* p_window_matrix, const int n_options, ...)
{
    MenuScreen* m = (MenuScreen *)malloc(sizeof(MenuScreen));

    m->pWindowMatrix = p_window_matrix;

    m->title = strBuild(title);

    m->charLenTitle = (int)strlen(m->title);

    m->numOptions = n_options;

    m->optionNames = (char **)malloc(n_options * sizeof(char *));

    m->currentlySelectedOptionIndex = 0;


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

// Menu constructor #2 (heap-allocated).
MenuScreen* initMenuScreenEmpty(const char* title, const float* p_window_matrix, int n_options)
{
    MenuScreen* m = (MenuScreen *)malloc(sizeof(MenuScreen));

    m->pWindowMatrix = p_window_matrix;

    m->title = strBuild(title);

    m->charLenTitle = (int)strlen(m->title);

    m->numOptions = n_options;

    m->optionNames = (char **)malloc(n_options * sizeof(char *));
    
    m->currentlySelectedOptionIndex = 0;


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


    float hiBorder = 0.5f + (m->numOptions / 2.0f + 1.0f) * .05f;
    float loBorder = 0.5f - (m->numOptions / 2.0f + 1.0f) * .05f;

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

    glColor4ub(255, 160, 160, 255);

    // Render menu's title.
    glRasterPos2f(
        0.5f * m->screenWidth - (m->charLenTitle + 0.5f) * 4.5f, 
        (hiBorder - .04f) * m->screenHeight
    );
    glutBitmapString(GLUT_BITMAP_9_BY_15, (unsigned char*)m->title);

    glColor4ub(255, 255, 255, 255);

    float offset = hiBorder - .09f;

    for (int i = 0; i < m->numOptions; ++i)
    {
        if (i == m->currentlySelectedOptionIndex)
        {
            // Render an arrow-like character slightly to the left of the curerent option's name.
            glRasterPos2f(0.46f * m->screenWidth, offset * m->screenHeight);
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '>');
        }

        // Render option name
        glRasterPos2f(0.48f * m->screenWidth, offset * m->screenHeight);
        glutBitmapString(
            GLUT_BITMAP_9_BY_15, 
            (const unsigned char*)m->optionNames[i]
        );

        // Move downwards
        offset -= .05f;
    }


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

const char* menuScreenHandler(MenuScreen* m, int* option_id)
{
    if (arrow_down_loaded)
    {
        if (m->currentlySelectedOptionIndex < m->numOptions - 1)
            m->currentlySelectedOptionIndex += 1;
        arrow_down_loaded = false;
    }

    if (arrow_up_loaded)
    {
        if (m->currentlySelectedOptionIndex > 0)
            m->currentlySelectedOptionIndex -= 1;
        arrow_up_loaded = false;
    }

    if (keystrokes['\n'] || keystrokes['\f'] || keystrokes['\r'])
    {
        if (option_id != NULL)
            *option_id = m->currentlySelectedOptionIndex;
        return m->optionNames[m->currentlySelectedOptionIndex];
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
    free(m->title);
    free(m);
}

#endif // MENU_SCREEN_H