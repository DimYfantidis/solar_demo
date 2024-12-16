#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

#include "custom_types.h"


struct Camera
{
    vector3f position;
    vector3f lookAt;
    vector3f upVector;
};
typedef struct Camera Camera;


Camera* initCamera(
    float pos_x, float pos_y, float pos_z,
    float ori_x, float ori_y, float ori_z,
    float up_x, float up_y, float up_z
)
{
    Camera* camera = (Camera *)malloc(sizeof(Camera));

    camera->position[0] = pos_x;
    camera->position[1] = pos_y;
    camera->position[2] = pos_z;
    
    camera->lookAt[0] = ori_x;
    camera->lookAt[1] = ori_y;
    camera->lookAt[2] = ori_z;

    float len = vectorLength3fv(camera->lookAt);

    camera->lookAt[0] /= len;
    camera->lookAt[1] /= len;
    camera->lookAt[2] /= len;

    camera->upVector[0] = up_x;
    camera->upVector[1] = up_y;
    camera->upVector[2] = up_z;

    return camera;
}

void updateCamera(Camera* camera)
{    
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60.0, 16.0 / 9.0, 0.1, 100.0);

    float x = camera->position[0];
    float y = camera->position[1];
    float z = camera->position[2];

    float dx = camera->lookAt[0];
    float dy = camera->lookAt[1];
    float dz = camera->lookAt[2];

    gluLookAt(
        x, y, z,
        x + dx, y + dy, z + dz,
        camera->upVector[0], camera->lookAt[1], camera->lookAt[2]
    );
}

#endif // CAMERA_H