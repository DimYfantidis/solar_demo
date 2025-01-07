#ifndef MOUSE_WHEEL_CALLBACK_H
#define MOUSE_WHEEL_CALLBACK_H


float move_speed_scale_factor;


void initModuleMouseWheelCallback(void)
{
    move_speed_scale_factor = 1.0f;
}

void callbackMouseWheel(int button, int dir, int x, int y)
{
    if (dir > 0)
    {
        move_speed_scale_factor *= 1.05f;
    }
    else
    {
        move_speed_scale_factor /= 1.05f;
    }
}

#endif // MOUSE_WHEEL_CALLBACK_H