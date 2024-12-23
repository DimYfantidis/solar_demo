#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/freeglut.h>

#include "Camera.h"
#include "CustomTypes.h"
#include "TextRendering.h"


struct MenuScreen
{
    Camera* POVanchor;
    char** optionNames;
    int numOptions;
    int currentlySelectedOption;
    float optionBoxWidth;
    float optionBoxHeight;
};
typedef struct MenuScreen MenuScreen;


MenuScreen* initMenuScreen(Camera* camera, int n_options, ...)
{
    MenuScreen* menuScreen = (MenuScreen *)malloc(sizeof(MenuScreen));

    menuScreen->POVanchor = camera;
    menuScreen->numOptions = n_options;
    menuScreen->optionNames = (char **)malloc(n_options * sizeof(char *));

    va_list optionStringsArgumentList;

    va_start(optionStringsArgumentList, n_options);

    for (int i = 0; i < n_options; ++i)
    {
        const char* option = va_arg(optionStringsArgumentList, const char*);

        menuScreen->optionNames[i] = strBuild(option);
    }

    va_end(optionStringsArgumentList);

    return menuScreen;
}

MenuScreen* initMenuScreenEmpty(Camera* camera, int n_options)
{
    MenuScreen* menuScreen = (MenuScreen *)malloc(sizeof(MenuScreen));

    menuScreen->POVanchor = camera;
    menuScreen->numOptions = n_options;
    menuScreen->optionNames = (char **)malloc(n_options * sizeof(char *));

    for (int i = 0; i < n_options; ++i) 
    {
        menuScreen->optionNames[i] = NULL;
    }

    return menuScreen;
}

bool setMenuScreenBoxDimensions(MenuScreen* menuScreen, float width, float height)
{
    if (width <= .0 || height <= .0 || width > 1. || height > 1.) {
        return false;
    }
    menuScreen->optionBoxWidth = width;
    menuScreen->optionBoxHeight = height;

    return true;
}

bool assignMenuScreenElement(MenuScreen* menuScreen, int idx, const char* option_at_idx)
{
    if (idx < 0 || idx > menuScreen->numOptions) {
        return false;
    }
    if (menuScreen->optionNames[idx] != NULL) {
        free(menuScreen->optionNames[idx]);
    }
    menuScreen->optionNames[idx] = strBuild(option_at_idx);

    return true;
}

void renderMenuScreen(MenuScreen* menuScreen)
{
    static const float spaceBetweenBoxes = 0.001f;

    float mainBoxWidth = menuScreen->optionBoxWidth + 2 * spaceBetweenBoxes;
    float mainBoxHeight = (menuScreen->numOptions + 1) * spaceBetweenBoxes
        + menuScreen->numOptions * menuScreen->optionBoxHeight;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    
    glLoadIdentity();

#define SCALE_X 0.0001f
#define SCALE_Y 0.0001f
#define SCALE_Z 0.0001f

    gluPerspective(60, 16 / 9, 0.01, 2.0 / SCALE_Z);

    gluLookAt(
        0, 0, 1,
        0, 0, -1,
        0, 1, 0
    );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glColor3f(0.2f, 0.2f, 0.2f);

    glLoadIdentity();
    glBegin(GL_QUADS);
    {
        glVertex3f(.05f / SCALE_X, (-0.02f * menuScreen->numOptions / 2) / SCALE_Y, -.5f / SCALE_Z);
        glVertex3f(-.05f / SCALE_X, (-0.02f * menuScreen->numOptions / 2) / SCALE_Y, -.5f / SCALE_Z);
        glVertex3f(-.05f / SCALE_X, (0.02f * menuScreen->numOptions / 2) / SCALE_Y, -.5f / SCALE_Z);
        glVertex3f(.05f / SCALE_X, (0.02f * menuScreen->numOptions / 2) / SCALE_Y, -.5f / SCALE_Z);
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    
    for (int i = 0; i < menuScreen->numOptions; ++i)
    {
        glLoadIdentity();
        glScalef(SCALE_X, SCALE_Y, SCALE_Z);
        unsigned char* opt = (unsigned char*)menuScreen->optionNames[i];
        glTranslatef(
            -0.05f / SCALE_X, 
            (-.03f * (i - menuScreen->numOptions / 2)) / SCALE_Y, 
            .2f / SCALE_Z);
        glutStrokeString(GLUT_STROKE_MONO_ROMAN, opt);
    }

#undef SCALE_X
#undef SCALE_Y
#undef SCALE_Z

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void deleteMenuScreen(MenuScreen* menuScreen)
{
    for (int i = 0; i < menuScreen->numOptions; ++i) {
        free(menuScreen->optionNames[i]);
    }
    free(menuScreen->optionNames);
    free(menuScreen);
}

#endif // MENU_SCREEN_H