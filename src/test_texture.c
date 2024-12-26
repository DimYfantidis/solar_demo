#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif 

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "Textures.h"


GLubyte* image;
GLuint textureID;

void display(void)
{
    static GLUquadric* sphere = NULL;
    static double theta = .0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 16.0 / 9.0, 0.1, 100.0);

    gluLookAt(
        10.0 * sin(theta), 5.0 * cos(theta) + 5.0, 10.0 * cos(theta),
        .0, .0, .0,
        .0, 1.0, .0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (sphere == NULL)
    {
        sphere = gluNewQuadric();
        gluQuadricTexture(sphere, GL_TRUE); // Enable texture coordinates for the quadric
    }

    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture

    glColor3f(1.0, 1.0, 1.0); // Set color to white to show the texture correctly
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluSphere(sphere, 3.0, 64, 32);


    glDisable(GL_TEXTURE_2D); // Disable 2D texturing after drawing

    glutSwapBuffers();

    theta += 0.003;

    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    printf("\n>>>>> Testing Program <<<<\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition((16.0 / 9.0) * 200.0, 200.0);
    int window_id = glutCreateWindow("Texture Testing");

    unsigned int width;
    unsigned int height;

    image = loadBitmapToRGBArray(
        "C:\\Users\\axaio\\Desktop\\PROJECTS\\my_solar_system\\data\\the_solar_system\\The Sun.bmp",
        &width,
        &height
    );

    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
    
    glGenTextures(1, &textureID); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); // Upload the texture data
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture

    glutDisplayFunc(display);

    free(image);

    glutMainLoop();

    return EXIT_SUCCESS;
}