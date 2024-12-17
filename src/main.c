// #define PROJ_DEBUG

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include "Camera.h"
#include "AmbientStars.h"
#include "StellarObject.h"
#include "KeyboardCallback.h"
#include "PassiveMotionCallback.h"


int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int WINDOW_ID;

double framerate;

clock_t refresh_ts;

StellarObject* sun;
StellarObject* earth;
StellarObject* moon;

Camera* camera;

AmbientStars* starsSkyBox;


void display(void)
{
    double elapsed_seconds = (double)(clock() - refresh_ts) / CLOCKS_PER_SEC;

    if (keystrokes[27])
    {
        glutDestroyWindow(WINDOW_ID);
        glutLeaveMainLoop();
        return;
    }

    if (elapsed_seconds < 1.0 / framerate) 
    {
        glutPostRedisplay();
        return;
    }
    refresh_ts = clock();

#ifdef PROJ_DEBUG
    printf("Clearing the screen (elapsed: %.3f sec)\n", elapsed_seconds);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    updateCamera(camera);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    renderStellarObject(sun);
    renderStellarObject(earth);
    renderStellarObject(moon);

    renderStars(starsSkyBox);

    glutSwapBuffers();
    glutPostRedisplay();
}

void initGlobals(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));

    WINDOW_WIDTH = 1920;
    WINDOW_HEIGHT = 1080;

    windowCentreX = WINDOW_WIDTH / 2;
    windowCentreY = WINDOW_HEIGHT / 2;

    CameraAngleHorizontal = .0f;
    CameraAngleVertical = .0f;

    mouseSensitivity = .002f;

    for (int i = 0; i < sizeof(keystrokes) / sizeof(keystrokes[0]); ++i) {
        keystrokes[i] = false;
    }

    framerate = 144.0;

    camera = initCamera(
        16.47074f, 32.79276f,  5.98598f,
        -0.444f, -0.881f, -0.163f,
        .0f, 1.0f, .0f
    );

    sun = colorise3ub(
        initStellarObject(2.0f, NULL, .0f, .0f, .0f),
        0xFF, 0x4D, 0x00
    );
    earth = colorise3f(
        initStellarObject(.4f, sun, .0f, 3.0f, .0f),
        .0f, .0f, 1.0f
    );
    moon = colorise3f(
        initStellarObject(0.06f, earth, .0f, .0f, 0.8f),
        1.0f, 1.0f, 1.0f
    );

    starsSkyBox = buildStars(400, camera);

    callbackPassiveMotion(windowCentreX, windowCentreY);
    callbackPassiveMotion(windowCentreX, windowCentreY);
}

void deallocateAll(void)
{
    deleteStellarObject(moon);
    deleteStellarObject(earth);
    deleteStellarObject(sun);

    deleteCamera(camera);

    deleteStars(starsSkyBox);

    glutExit();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	WINDOW_ID = glutCreateWindow("Solar System - exhibition");

    glutFullScreen();

	glClearColor(
        0.0196078431372549f / 4, 
        0.029411764705882353f / 3, 
        0.0803921568627451f / 3, 
        1.0f
    );
	glEnable(GL_DEPTH_TEST);

    initGlobals(argc, argv); 
    printf("[1] >>> Hello, Universe!\n");

    glutSetCursor(GLUT_CURSOR_NONE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutDisplayFunc(display);
    glutKeyboardFunc(callbackKeyboard);
    glutKeyboardUpFunc(callbackKeyboardUp);
    glutPassiveMotionFunc(callbackPassiveMotion);


    {

	    glutMainLoop();
    
    }


    printf("[2] >>> Exited Main Loop.\n");

    deallocateAll();

    printf("[3] >>> Deallocated all memory.\n");

    return 0;
}
