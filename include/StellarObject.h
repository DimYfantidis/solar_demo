#ifndef STELLAR_OBJECT_H
#define STELLAR_OBJECT_H

#include <string.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#include "custom_types.h"
#include "TextRendering.h"


struct StellarObject
{
    char* name;
    GLUquadric* quad;
    struct StellarObject* parent;
    vector3f position;
    float radius;
    float parametric_angle;
    float velocity;
    float parent_dist;
    float solar_tilt;
    vector3ub color;
};
typedef struct StellarObject StellarObject;


StellarObject* initStellarObject(
    const char* name, 
    float radius, 
    float vel, 
    StellarObject* parent, 
    float parent_dist,
    float solar_tilt
) 
{
#ifdef PROJ_DEBUG
    printf("Creating StellarObject of radius %.2f\n", radius);
#endif
    StellarObject* p = (StellarObject*)malloc(sizeof(StellarObject));

    p->name = strBuild(name);
    p->radius = radius;
    p->parent = parent;
    p->solar_tilt = solar_tilt;
    p->quad = gluNewQuadric();
    p->parametric_angle = (float)(-M_PI);
    p->velocity = vel;

    gluQuadricDrawStyle(p->quad, GLU_FILL);
    
    p->parent_dist = parent_dist;

    memset(p->position, (int).0f, sizeof(p->position));
    memset(p->position, (int)0x00, sizeof(p->position));

    return p;
}

StellarObject* colorise3f(StellarObject* p, float red, float green, float blue)
{
    p->color[0] = (ubyte)(red * 255);
    p->color[1] = (ubyte)(green * 255);
    p->color[2] = (ubyte)(blue * 255);
    
    return p;
}

StellarObject* colorise3ub(StellarObject* p, ubyte red, ubyte green, ubyte blue)
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
    glColor3ubv(p->color);
    glPushMatrix();

    glTranslatef(
        p->position[0], 
        p->position[1], 
        p->position[2]
    );

    glRotatef(p->solar_tilt, 0, 0, 1);

    glTranslatef(
        p->parent->position[0],
        p->parent->position[1],
        p->parent->position[2]
    );
    
    // Render planet
    gluSphere(p->quad, p->radius, 64, 32);

    // Render planet's nametag
    renderStringInWorld(
        .0f, p->radius * 1.1f, .0f,
        GLUT_BITMAP_9_BY_15, p->name,
        0xFF, 0xFF, 0xFF
    );

    glPopMatrix();
}

#endif // STELLAR_OBJECT_H