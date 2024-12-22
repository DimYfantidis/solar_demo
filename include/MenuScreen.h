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
    char** optionNames;
    int numOptions;
    int currentlySelectedOption;
    float optionBoxWidth;
    float optionBoxHeight;
};
typedef struct MenuScreen MenuScreen;


MenuScreen* initMenuScreen(int n_options, ...)
{
    MenuScreen* menuScreen = (MenuScreen *)malloc(sizeof(MenuScreen));

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

MenuScreen* initMenuScreenEmpty(int n_options)
{
    MenuScreen* menuScreen = (MenuScreen *)malloc(sizeof(MenuScreen));

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
    return;

    static const float spaceBetweenBoxes = 0.001f;

    float mainBoxWidth = menuScreen->optionBoxWidth + 2 * spaceBetweenBoxes;
    float mainBoxHeight = (menuScreen->numOptions + 1) * spaceBetweenBoxes
        + menuScreen->numOptions * menuScreen->optionBoxHeight;

    for (int i = 1; i < menuScreen->numOptions; ++i)
    {
        // ...
    }
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