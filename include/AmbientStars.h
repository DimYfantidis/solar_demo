#ifndef AMBIENT_STARS_H
#define AMBIENT_STARS_H


#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif 

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "Camera.h"
#include "Textures.h"


typedef struct AmbientStars
{
    // The centre of the sphere with stars.
    Camera* POVanchor;

    int numberOfStars;

    vector3r* positions;

    GLUquadric** quads;

    real_t sizeInWorld;

    GLuint texture;

} AmbientStars;


AmbientStars* buildStars(const int numberOfStars, Camera* POVanchor)
{
    AmbientStars* stars = (AmbientStars *)malloc(sizeof(AmbientStars));

    stars->numberOfStars = numberOfStars;
    
    stars->positions = (vector3r*)malloc(numberOfStars * sizeof(vector3r));

    stars->POVanchor = POVanchor;

    stars->sizeInWorld = (real_t)POVanchor->renderDistance * 0.0007;

    stars->quads = (GLUquadric **)malloc(numberOfStars * sizeof(GLUquadric *));

    stars->texture = 0;

    for (int i = 0; i < stars->numberOfStars; ++i)
    {
        stars->quads[i] = gluNewQuadric();
        gluQuadricDrawStyle(stars->quads[i], GLU_FILL);

        real_t x = (real_t)(2.0 * ((double)rand() / RAND_MAX) - 1.0);
        real_t y = (real_t)(2.0 * ((double)rand() / RAND_MAX) - 1.0);
        real_t z = (real_t)(2.0 * ((double)rand() / RAND_MAX) - 1.0);

        real_t len = (real_t)sqrt((double)(x * x + y * y + z * z));

        x /= len;
        y /= len;
        z /= len;

        stars->positions[i][0] = (real_t)(x * (POVanchor->renderDistance * (real_t)0.8));
        stars->positions[i][1] = (real_t)(y * (POVanchor->renderDistance * (real_t)0.8));
        stars->positions[i][2] = (real_t)(z * (POVanchor->renderDistance * (real_t)0.8));
    }

    return stars;
}

AmbientStars* buildStarsFromTexture(const char* data_dir, Camera* POVanchor)
{
    AmbientStars* stars = (AmbientStars *)malloc(sizeof(AmbientStars));

    stars->numberOfStars = 1;
    
    stars->positions = NULL;

    stars->POVanchor = POVanchor;

    stars->sizeInWorld = (real_t).0;

    stars->quads = (GLUquadric **)malloc(sizeof(GLUquadric *));

    stars->quads[0] = gluNewQuadric();
    gluQuadricTexture(stars->quads[0], GL_TRUE);

    char* texture_filename = strCat(2, data_dir, "SKYBOX.bmp");

    if (!registerTexture(texture_filename, &stars->texture))
    {
        free(texture_filename);
        free(stars);
        return NULL;
    }
    free(texture_filename);

    return stars;
}

void deleteStars(AmbientStars* stars)
{
    if (stars == NULL)
        return;

    for (int i = 0; i < stars->numberOfStars; ++i)
        gluDeleteQuadric(stars->quads[i]);

    free(stars->quads);

    if (stars->positions == NULL)
        glDeleteTextures(1, &stars->texture);
    else
        free(stars->positions);
}

void renderStars(AmbientStars* stars)
{
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glLoadIdentity();

    if (stars->positions != NULL)
    {
        glColor3f(1.0f, 1.0f, 1.0f);

        // Low Resolution Sky rendering.
        for (int i = 0; i < stars->numberOfStars; ++i)
        {
            glPushMatrix();

            glTranslatef(
                (float)stars->POVanchor->position[0], 
                (float)stars->POVanchor->position[1],
                (float)stars->POVanchor->position[2]
            );

            glTranslatef(
                (float)stars->positions[i][0], 
                (float)stars->positions[i][1],
                (float)stars->positions[i][2]
            );

            gluSphere(stars->quads[i], stars->sizeInWorld, 3, 3);

            glPopMatrix();
        }
    }
    else
    {
        // High Resolution Sky rendering.   
        glColor3f(0.3f, 0.3f, 0.3f);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, stars->texture);

        glTranslatef(
            (float)stars->POVanchor->position[0], 
            (float)stars->POVanchor->position[1],
            (float)stars->POVanchor->position[2]
        );

        glRotatef(100.0f, 0.957826f, -0.287348f, 0.0f);

        gluSphere(
            stars->quads[0], 
            (double)stars->POVanchor->renderDistance * 0.8, 
            128, 64
        );

        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

#endif // AMBIENT_STARS_H
