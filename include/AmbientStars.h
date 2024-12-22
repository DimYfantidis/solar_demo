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


struct AmbientStars
{
    Camera* POVanchor;
    int numberOfStars;
    vector3f* positions;
    GLUquadric** quads;
    float size_in_world;
};
typedef struct AmbientStars AmbientStars;


AmbientStars* buildStars(const int numberOfStars, Camera* POVanchor)
{
    AmbientStars* stars = (AmbientStars *)malloc(sizeof(AmbientStars));

    stars->numberOfStars = numberOfStars;
    
    stars->positions = (vector3f*)malloc(numberOfStars * sizeof(vector3f));

    stars->POVanchor = POVanchor;

    stars->size_in_world = POVanchor->renderDistance * 0.0007f;

    stars->quads = (GLUquadric **)malloc(numberOfStars * sizeof(GLUquadric *));

    for (int i = 0; i < stars->numberOfStars; ++i)
    {
        stars->quads[i] = gluNewQuadric();
        gluQuadricDrawStyle(stars->quads[i], GLU_FILL);

        float x = (float)(2.0 * ((double)rand() / RAND_MAX) - 1.0);
        float y = (float)(2.0 * ((double)rand() / RAND_MAX) - 1.0);
        float z = (float)(2.0 * ((double)rand() / RAND_MAX) - 1.0);

        float len = sqrtf(x * x + y * y + z * z);

        x /= len;
        y /= len;
        z /= len;

        stars->positions[i][0] = x * (POVanchor->renderDistance * 0.8f);
        stars->positions[i][1] = y * (POVanchor->renderDistance * 0.8f);
        stars->positions[i][2] = z * (POVanchor->renderDistance * 0.8f);
    }

    return stars;
}

void deleteStars(AmbientStars* stars)
{
    if (stars == NULL)
        return;

    for (int i = 0; i < stars->numberOfStars; ++i)
    {
        gluDeleteQuadric(stars->quads[i]);
    }
    free(stars->quads);
    free(stars->positions);
}

void renderStars(AmbientStars* stars)
{
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < stars->numberOfStars; ++i)
    {
        glPushMatrix();
        glTranslatef(
            stars->positions[i][0] + stars->POVanchor->position[0], 
            stars->positions[i][1] + stars->POVanchor->position[1],
            stars->positions[i][2] + stars->POVanchor->position[2]
        );
        gluSphere(stars->quads[i], stars->size_in_world, 3, 3);
        glPopMatrix();
    }
}

#endif // AMBIENT_STARS_H
