#define PROJ_DEBUG

#include <stdio.h>
#include <GL/glut.h>

#include "StellarObject.h"


int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 1000;

StellarObject* sun;
StellarObject* earth;
StellarObject* moon;


void display(void)
{
    float radius = 20.0f;

    printf("Clearing the screen\n");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-radius, radius, -radius, radius, -radius - 40.0, radius + 40.0);

    gluLookAt(
        16.0, 0.0, 12.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0
    );

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

    sun = colorise3f(
        initStellarObject(2.0f, NULL, .0f, .0f, .0f),
        1.0f, 1.0f, .0f
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