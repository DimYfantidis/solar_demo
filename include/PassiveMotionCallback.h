#ifndef PASSIVE_MOTION_CALLBACK_H
#define PASSIVE_MOTION_CALLBACK_H

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#   define _USE_MATH_DEFINES
#endif 

#include <math.h>
#include <GL/glut.h>

#include "CustomTypes.h"
 

float camera_angle_horizontal;
float camera_angle_vertical;

int window_centre_X;
int window_centre_Y;

float mouse_sensitivity;


void callbackPassiveMotion(int x, int y)
{
    static int x_prev = 0;
    static int y_prev = 0;

    float dx = (float)(x - x_prev) * mouse_sensitivity;
    float dy = (float)(y - y_prev) * mouse_sensitivity;

    x_prev = window_centre_X;
    y_prev = window_centre_Y;

    camera_angle_horizontal = camera_angle_horizontal - dx;
    camera_angle_vertical = clampf(camera_angle_vertical - dy, (float)(-M_PI_2  * 0.95), (float)(M_PI_2 * 0.95));

    glutWarpPointer(window_centre_X, window_centre_Y);
    glutPostRedisplay();
}

#endif // PASSIVE_MOTION_CALLBACK_H