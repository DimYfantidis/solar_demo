#ifndef KEYBOARD_CALLBACK_H
#define KEYBOARD_CALLBACK_H

#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <GL/glut.h>

#include "Timer.h"


bool keystrokes[256];
bool shift_key_down = false;
bool ctrl_key_down = false;
bool alt_key_down = false;

bool arrowDownLoaded = false;
bool arrowUpLoaded = false;


void callbackKeyboard(unsigned char key, int x, int y)
{
    int modifiers = glutGetModifiers();

    shift_key_down = ((modifiers & GLUT_ACTIVE_SHIFT) != 0 ? true : false);
    alt_key_down = ((modifiers & GLUT_ACTIVE_ALT) != 0 ? true : false);
    ctrl_key_down = ((modifiers & GLUT_ACTIVE_CTRL) != 0 ? true : false);

    keystrokes[toupper(key)] = true;
}

void callbackKeyboardUp(unsigned char key, int x, int y)
{
    keystrokes[toupper(key)] = false;
}

void callbackSpecialKeyboard(int key, int x, int y)
{
    static const uint64_t cooldown_ms = 100;
    
    static uint64_t cooldown_ts = 0;
    static uint64_t current_ms = 0;


    if ((current_ms = getAbsoluteTimeMillis()) - cooldown_ts >= cooldown_ms)
    {
        cooldown_ts = current_ms;

        if (key == GLUT_KEY_DOWN)
            arrowDownLoaded = true;
        
        if (key == GLUT_KEY_UP)
            arrowUpLoaded = true;
    }
}


// When the specified key is pressed, the boolean variable alternates between true and false every `timeout_ms` milliseconds.
void keyToggle(char key, bool* toggleVar, uint64_t timeout_ms)
{
    static bool initialised = false;
    static uint64_t timestampPrev[256];

    if (!initialised)
    {
        for (int i = 0; i < sizeof(timestampPrev) / sizeof(timestampPrev[0]); ++i)
            timestampPrev[i] = 0;
        
        initialised = true;
    }

    uint64_t elapsed_ms = getAbsoluteTimeMillis() - timestampPrev[key];

    if (keystrokes[key] && elapsed_ms >= timeout_ms)
    {
        timestampPrev[key] = getAbsoluteTimeMillis();
        *toggleVar = !(*toggleVar);
    }
}


#endif // KEYBOARD_CALLBACK_H