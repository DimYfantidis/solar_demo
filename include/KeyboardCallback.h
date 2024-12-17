#ifndef KEYBOARD_CALLBACK_H
#define KEYBOARD_CALLBACK_H

#include <ctype.h>
#include <stdbool.h>
#include <GL/glut.h>


bool keystrokes[256];
bool shift_key_down = false;
bool ctrl_key_down = false;
bool alt_key_down = false;

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

}


#endif // KEYBOARD_CALLBACK_H