#ifndef STELLAR_OBJECT_H
#define STELLAR_OBJECT_H

#include <string.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#include "custom_types.h"
#include "TextRendering.h"


struct StellarObject
{
    float radius;
    float parametric_angle;
    float velocity;
    float parent_dist;
    char* name;
    GLUquadric* quad;
    struct StellarObject* parent;
    vector3f position;
    vector3ub color;
};
typedef struct StellarObject StellarObject;


StellarObject* initStellarObject(const char* name, float radius, float vel, StellarObject* parent, float parent_dist) 
{
#ifdef PROJ_DEBUG
    printf("Creating StellarObject of radius %.2f\n", radius);
#endif
    StellarObject* p = (StellarObject*)malloc(sizeof(StellarObject));

    p->name = strBuild(name);
    p->radius = radius;
    p->parent = parent;
    p->quad = gluNewQuadric();
    p->parametric_angle = (float)(-M_PI);
    p->velocity = vel;

    gluQuadricDrawStyle(p->quad, GLU_FILL);
    
    p->parent_dist = parent_dist;
    
#ifdef PROJ_DEBUG
    printf("StellarObject created at (%.3f, %.3f, %.3f)\n", p->position[0], p->position[1], p->position[2]);
#endif
    p->color[0] = p->color[1] = p->color[2] = 0x00;

    return p;
}

StellarObject* colorise3f(StellarObject* p, float red, float green, float blue)
{
    p->color[0] = (byte)(red * 255);
    p->color[1] = (byte)(green * 255);
    p->color[2] = (byte)(blue * 255);
    
    return p;
}

StellarObject* colorise3ub(StellarObject* p, byte red, byte green, byte blue)
{
    p->color[0] = red;
    p->color[1] = green;
    p->color[2] = blue;
    
    return p;
}


void deleteStellarObject(StellarObject* p)
{
    gluDeleteQuadric(p->quad);
    free(p->name);
    free(p);
}

void updateStellarObject(StellarObject* p)
{
    p->parametric_angle += 2.0f * p->velocity;

    if (p->parametric_angle > M_PI) {
        p->parametric_angle -= (float)(2 * M_PI);
    }

    float sin_ang = sinf(p->parametric_angle);
    float cos_ang = cosf(p->parametric_angle);

    p->position[0] = cos_ang * p->parent_dist;
    p->position[2] = sin_ang * p->parent_dist;
}

void renderStellarObject(StellarObject* p)
{
    renderStringInWorld(

        p->position[0] + p->parent->position[0], 
        p->position[1] + p->parent->position[1] + p->radius * 1.1f, 
        p->position[2] + p->parent->position[2],

        GLUT_BITMAP_9_BY_15, p->name,
        0xFF, 0xFF, 0xFF
    );

    glColor3ubv(p->color);
    glPushMatrix();
    glTranslatef(
        p->parent->position[0],
        p->parent->position[1],
        p->parent->position[2]
    );
    glTranslatef(
        p->position[0], 
        p->position[1], 
        p->position[2]
    );
    gluSphere(p->quad, p->radius, 64, 32);
    glPopMatrix();
}

#endif // STELLAR_OBJECT_H