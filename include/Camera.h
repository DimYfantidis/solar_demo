#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

#include "custom_types.h"
#include "KeyboardCallback.h"
#include "PassiveMotionCallback.h"


struct Camera
{
    vector3f position;
    vector3f lookAt;
    vector3f upVector;
    float movementSpeed;
    float renderDistance;
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

    camera->movementSpeed = 0.5f;
    camera->renderDistance = 1000.0f;

    return camera;
}

void updateCamera(Camera* camera)
{   
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60.0, 16.0 / 9.0, 0.1, (double)camera->renderDistance);

    float sin_vert = sinf(CameraAngleVertical);
	float sin_horz = sinf(CameraAngleHorizontal);
	float cos_vert = cosf(CameraAngleVertical);
	float cos_horz = cosf(CameraAngleHorizontal);

    // Camera direction, defined by the unary sphere.
    camera->lookAt[0] = cos_vert * sin_horz;
    camera->lookAt[1] = sin_vert;
    camera->lookAt[2] = cos_vert * cos_horz;

#ifdef PROJ_DEBUG
    printf(
        "Camera Orientation is: (%.3f, %.3f, %.3f)\n", 
        camera->lookAt[0],
        camera->lookAt[1],
        camera->lookAt[2]
    );
#endif

    float movementSpeed = camera->movementSpeed;

    if (shift_key_down) 
    {
        movementSpeed /= 3;
    }

    if (keystrokes['W'])
    {
        camera->position[0] += camera->lookAt[0] * movementSpeed;
        camera->position[1] += camera->lookAt[1] * movementSpeed;
        camera->position[2] += camera->lookAt[2] * movementSpeed;
    }
    if (keystrokes['S'])
    {
        camera->position[0] -= camera->lookAt[0] * movementSpeed;
        camera->position[1] -= camera->lookAt[1] * movementSpeed;
        camera->position[2] -= camera->lookAt[2] * movementSpeed;
    }
    if (keystrokes['A'])
    {
        camera->position[0] += cos_horz * movementSpeed;
        camera->position[2] -= sin_horz * movementSpeed;
    }
    if (keystrokes['D'])
    {
        camera->position[0] -= cos_horz * movementSpeed;
        camera->position[2] += sin_horz * movementSpeed;
    }
    if (keystrokes['X'])
    {
        camera->position[1] -= movementSpeed;
    }
    if (keystrokes[' '])
    {
        camera->position[1] += movementSpeed;
    }

    gluLookAt(
        camera->position[0], 
        camera->position[1], 
        camera->position[2],

        camera->position[0] + camera->lookAt[0], 
        camera->position[1] + camera->lookAt[1], 
        camera->position[2] + camera->lookAt[2],

        camera->upVector[0], 
        camera->upVector[1], 
        camera->upVector[2]
    );
}

inline void deleteCamera(Camera* camera)
{
    free(camera);
}

#endif // CAMERA_H