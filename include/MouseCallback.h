#ifndef MOUSE_CALLBACK_H
#define MOUSE_CALLBACK_H

#include <stdio.h>
#include <GL/glut.h>

#include "CustomTypes.h"


void callbackMouse(int button, int state, int x, int y)
{
    char* state_str = strBuild(state == GLUT_UP ? "UP" : "DOWN");

    if (button == GLUT_LEFT_BUTTON)
    {
        printf("LMB %s\n", state_str);
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        printf("RMB %s\n", state_str);
    }
    else if (button == GLUT_MIDDLE_BUTTON)
    {
        printf("MWH %s\n", state_str);
    }

    free(state_str);
}


#endif // MOUSE_CALLBACK_H
