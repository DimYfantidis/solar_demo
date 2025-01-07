#ifndef KEYBOARD_CALLBACK_H
#define KEYBOARD_CALLBACK_H

#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <GL/glut.h>

#include "Timer.h"


bool keystrokes[256];
bool shift_key_down;
bool ctrl_key_down;
bool alt_key_down;

bool arrow_down_loaded;
bool arrow_up_loaded;


void initModuleKeyboardCallback(void)
{
    shift_key_down = false;
    ctrl_key_down = false;
    alt_key_down = false;

    arrow_down_loaded = false;
    arrow_up_loaded = false;

    // Used for keyboard input
    for (int i = 0; i < sizeof(keystrokes) / sizeof(keystrokes[0]); ++i)
        keystrokes[i] = false;
}


void callbackKeyboardDown(unsigned char key, int x, int y)
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
            arrow_down_loaded = true;
        
        if (key == GLUT_KEY_UP)
            arrow_up_loaded = true;
    }
}


// When the specified key is pressed, the boolean variable alternates between true and false every `timeout_ms` milliseconds.
void keyToggle(char key, bool* toggle_var, uint64_t timeout_ms)
{
    static bool initialised = false;
    static uint64_t timestamp_prev[256];

    if (!initialised)
    {
        for (int i = 0; i < sizeof(timestamp_prev) / sizeof(timestamp_prev[0]); ++i)
            timestamp_prev[i] = 0;
        
        initialised = true;
    }

    uint64_t elapsed_ms = getAbsoluteTimeMillis() - timestamp_prev[key];

    if (keystrokes[key] && elapsed_ms >= timeout_ms)
    {
        timestamp_prev[key] = getAbsoluteTimeMillis();
        *toggle_var = !(*toggle_var);
    }
}


#endif // KEYBOARD_CALLBACK_H