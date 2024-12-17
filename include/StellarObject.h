#ifndef STELLAR_OBJECT_H
#define STELLAR_OBJECT_H

#include <stdlib.h>
#include <GL/freeglut.h>

#include "custom_types.h"


struct StellarObject
{
    float radius;
    GLUquadric* quad;
    vector3f color;
    vector3f position;
    struct StellarObject* parent;
};
typedef struct StellarObject StellarObject;


StellarObject* initStellarObject(float radius, StellarObject* parent, float dx, float dy, float dz) 
{
#ifdef PROJ_DEBUG
    printf("Creating StellarObject of radius %.2f\n", radius);
#endif
    StellarObject* p = (StellarObject*)malloc(sizeof(StellarObject));

    p->radius = radius;
    p->parent = parent;
    p->quad = gluNewQuadric();

    gluQuadricDrawStyle(p->quad, GLU_FILL);

    if (parent != NULL)
    {
        p->position[0] = parent->position[0] + dx;
        p->position[1] = parent->position[1] + dy;
        p->position[2] = parent->position[2] + dz;
    }
    else 
    {
        p->position[0] = dx;
        p->position[1] = dx;
        p->position[2] = dx;
    }
#ifdef PROJ_DEBUG
    printf("StellarObject created at (%.3f, %.3f, %.3f)\n", p->position[0], p->position[1], p->position[2]);
#endif
    p->color[0] = p->color[1] = p->color[2] = .0f;

    return p;
}

StellarObject* colorise3f(StellarObject* instance, float red, float green, float blue)
{
    instance->color[0] = red;
    instance->color[1] = green;
    instance->color[2] = blue;
    
    return instance;
}

StellarObject* colorise3ub(StellarObject* instance, unsigned char red, unsigned char green, unsigned char blue)
{
    instance->color[0] = red / 255.0f;
    instance->color[1] = green / 255.0f;
    instance->color[2] = blue / 255.0f;
    
    return instance;
}


void deleteStellarObject(StellarObject* instance)
{
    gluDeleteQuadric(instance->quad);
    free(instance);
}

void renderStellarObject(StellarObject* instance)
{
    glColor3fv(instance->color);
    glPushMatrix();
    glTranslatef(instance->position[0], instance->position[1], instance->position[2]);
    gluSphere(instance->quad, instance->radius, 64, 32);
    glPopMatrix();
}

#endif // STELLAR_OBJECT_H