#define PROJ_DEBUG

#include <time.h>
#include <stdio.h>
#include <GL/glut.h>

#include "Camera.h"
#include "StellarObject.h"


int WINDOW_WIDTH = 1920;
int WINDOW_HEIGHT = 1080;

double framerate = 144.0;

clock_t refresh_ts;

StellarObject* sun;
StellarObject* earth;
StellarObject* moon;

Camera* camera;


void display(void)
{
    if ((double)(clock() - refresh_ts) / CLOCKS_PER_SEC < 1 / framerate) 
    {
        return;
    }
    refresh_ts = clock();

    printf("Clearing the screen\n");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    updateCamera(camera);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    renderStellarObject(sun);
    renderStellarObject(earth);
    renderStellarObject(moon);

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	int window_id = glutCreateWindow("Solar System - exhibition");

	glClearColor(0.0196078431372549, 0.029411764705882353, 0.0803921568627451, 1.0f);
	glEnable(GL_DEPTH_TEST);

    printf("Hello, World?\n");

    camera = initCamera(
        16.0f, .0f, 12.0f,
        -16.0f, .0f, -12.0f,
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

    glutDisplayFunc(display);
	glutMainLoop();

    return 0;
}