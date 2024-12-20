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

    stars->size_in_world = POVanchor->renderDistance * 0.001f;

    stars->quads = (GLUquadric **)malloc(numberOfStars * sizeof(GLUquadric *));

    for (int i = 0; i < stars->numberOfStars; ++i)
    {
        stars->quads[i] = gluNewQuadric();
        gluQuadricDrawStyle(stars->quads[i], GLU_FILL);

        float u = (float)(2.0 * M_PI * ((double)rand() / RAND_MAX) - M_PI);
        float v = (float)(2.0 * M_PI * ((double)rand() / RAND_MAX) - M_PI);

        float sin_vert = sinf(u);
        float sin_horz = sinf(v);
        float cos_vert = cosf(u);
        float cos_horz = cosf(v);

        stars->positions[i][0] = cos_vert * sin_horz * (POVanchor->renderDistance * 0.95f);
        stars->positions[i][1] = sin_vert * (POVanchor->renderDistance * 0.95f);
        stars->positions[i][2] = cos_vert * cos_horz * (POVanchor->renderDistance * 0.95f);
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
