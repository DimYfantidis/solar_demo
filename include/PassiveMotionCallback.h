#ifndef PASSIVE_MOTION_CALLBACK_H
#define PASSIVE_MOTION_CALLBACK_H

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#   define _USE_MATH_DEFINES
#endif 

#include <math.h>
#include <GL/glut.h>

#include "CustomTypes.h"
 

float CameraAngleHorizontal;
float CameraAngleVertical;

int windowCentreX;
int windowCentreY;

float mouseSensitivity;


void callbackPassiveMotion(int x, int y)
{
    static int x_prev = 0;
    static int y_prev = 0;

    float dx = (float)(x - x_prev) * mouseSensitivity;
    float dy = (float)(y - y_prev) * mouseSensitivity;

    x_prev = windowCentreX;
    y_prev = windowCentreY;

    CameraAngleHorizontal = CameraAngleHorizontal - dx;
    CameraAngleVertical = clampf(CameraAngleVertical - dy, (float)(-M_PI_2  * 0.95), (float)(M_PI_2 * 0.95));

    glutWarpPointer(windowCentreX, windowCentreY);
    glutPostRedisplay();
}

#endif // PASSIVE_MOTION_CALLBACK_H