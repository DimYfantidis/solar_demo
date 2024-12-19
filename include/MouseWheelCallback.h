#ifndef MOUSE_WHEEL_CALLBACK_H
#define MOUSE_WHEEL_CALLBACK_H


float moveSpeedScaleFactor = 1.0f;

void callbackMouseWheel(int button, int dir, int x, int y)
{
    if (dir > 0)
    {
        moveSpeedScaleFactor *= 1.05f;
    }
    else
    {
        moveSpeedScaleFactor /= 1.05f;
    }
}

#endif // MOUSE_WHEEL_CALLBACK_H