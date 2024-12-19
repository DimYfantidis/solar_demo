#ifndef STELLAR_OBJECT_H
#define STELLAR_OBJECT_H

#include <string.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#include "custom_types.h"
#include "TextRendering.h"
#include "KeyboardCallback.h"


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
    p->parent_dist = parent_dist;
    p->velocity = vel;

    if (parent_dist != .0f) {
        p->velocity /= parent_dist;
    }

    memset(p->position, (int).0f, sizeof(p->position));
    memset(p->position, (int)0x00, sizeof(p->position));

    gluQuadricDrawStyle(p->quad, GLU_FILL);

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

void updateStellarObject(StellarObject* p, float speed_factor)
{
    p->parametric_angle += speed_factor * p->velocity;

    if (p->parametric_angle > M_PI) {
        p->parametric_angle -= (float)(2 * M_PI);
    }

    float sin_ang = sinf(p->parametric_angle);
    float cos_ang = cosf(p->parametric_angle);

    p->position[0] = cos_ang * p->parent_dist;
    p->position[2] = sin_ang * p->parent_dist;
}

StellarObject** getStellarObjectAncestors(StellarObject* p, int* n_ancestors)
{
    StellarObject* cntr_ptr = p->parent;

    *n_ancestors = 0;

    while (cntr_ptr != NULL)
    {
        *n_ancestors += 1;
        cntr_ptr = cntr_ptr->parent;
    }
    if (*n_ancestors == 0) {
        return NULL;
    }

    StellarObject** ancestors = (StellarObject **)malloc(*n_ancestors * sizeof(StellarObject *));

    cntr_ptr = p->parent;

    for (int i = 0; i < *n_ancestors; ++i)
    {
        ancestors[i] = cntr_ptr;
        cntr_ptr = cntr_ptr->parent;
    }
    return ancestors;
}

void renderStellarObject(StellarObject* p, bool render_trajectory, StellarObject** ancestors, int* n_ancestors)
{
    bool uses_cached_ancestors = (ancestors == NULL ? false : true);

    int tmp_n_ancestors;


    glColor3ubv(p->color);
    glPushMatrix();

    if (n_ancestors == NULL)
        n_ancestors = &tmp_n_ancestors;

    if (!uses_cached_ancestors)
        ancestors = getStellarObjectAncestors(p, n_ancestors);

    // Apply the transformations of all previous astrological systems' anchors. 
    for (int i = *n_ancestors - 1; i >= 0; --i) 
    {
        glRotatef(ancestors[i]->solar_tilt, 0, 0, 1);

        glTranslatef(
            ancestors[i]->position[0],
            ancestors[i]->position[1],
            ancestors[i]->position[2]
        );
    }

    if (!uses_cached_ancestors)
        free(ancestors);

    glRotatef(p->solar_tilt, 0, 0, 1);
    
    // The above transformations are saved as they will be 
    // used for rendering the StellarObject's trajectory as well.
    glPushMatrix();

    glTranslatef(
        p->position[0], 
        p->position[1], 
        p->position[2]
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

    if (render_trajectory)
    {
        glColor4f(p->color[0], p->color[1], p->color[2], 0.15f);

        glScalef(p->parent_dist, p->parent_dist, p->parent_dist);

        glBegin(GL_LINE_LOOP);
        for (float theta = (float)(-M_PI); theta < (float)M_PI; theta += (float)M_PI / 100.0f) 
        {
            glVertex3f(cosf(theta), .0f, sinf(theta));
        }
        glEnd();
    }
    glPopMatrix();
}

#endif // STELLAR_OBJECT_H