#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

#include "CustomTypes.h"
#include "KeyboardCallback.h"
#include "PassiveMotionCallback.h"


typedef struct Camera
{
    vector3r position;

    vector3r lookAt;

    vector3r upVector;

    real_t movementSpeed;

    real_t renderDistance;
    
} Camera;


Camera* initCamera(
    double pos_x, double pos_y, double pos_z,
    double ori_x, double ori_y, double ori_z,
    double up_x, double up_y, double up_z,
    double render_distance
)
{
    Camera* camera = (Camera *)malloc(sizeof(Camera));

    camera->position[0] = (real_t)pos_x;
    camera->position[1] = (real_t)pos_y;
    camera->position[2] = (real_t)pos_z;
    
    camera->lookAt[0] = (real_t)ori_x;
    camera->lookAt[1] = (real_t)ori_y;
    camera->lookAt[2] = (real_t)ori_z;

    real_t len = vectorLength3rv(camera->lookAt);

    // Regularization, if needed.
    camera->lookAt[0] /= len;
    camera->lookAt[1] /= len;
    camera->lookAt[2] /= len;

    camera->upVector[0] = (real_t)up_x;
    camera->upVector[1] = (real_t)up_y;
    camera->upVector[2] = (real_t)up_z;

    camera->movementSpeed = (real_t)0.5;
    camera->renderDistance = (real_t)render_distance;

    return camera;
}

void updateCamera(Camera* camera)
{
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60.0, 16.0 / 9.0, 0.01, (double)camera->renderDistance);

    double sin_vert = sin((double)CameraAngleVertical);
	double sin_horz = sin((double)CameraAngleHorizontal);
	double cos_vert = cos((double)CameraAngleVertical);
	double cos_horz = cos((double)CameraAngleHorizontal);

    // Camera direction, defined by the unary sphere.
    camera->lookAt[0] = (real_t)(cos_vert * sin_horz);
    camera->lookAt[1] = (real_t)sin_vert;
    camera->lookAt[2] = (real_t)(cos_vert * cos_horz);

    real_t movementSpeed = camera->movementSpeed;

    if (shift_key_down) 
    {
        movementSpeed *= (real_t)10.0;
    }

    if (keystrokes['W'])
    {
        // Forward movement (towards the direction of the camera).
        camera->position[0] += camera->lookAt[0] * movementSpeed;
        camera->position[1] += camera->lookAt[1] * movementSpeed;
        camera->position[2] += camera->lookAt[2] * movementSpeed;
    }
    if (keystrokes['S'])
    {
        // Backwards movement.
        camera->position[0] -= camera->lookAt[0] * movementSpeed;
        camera->position[1] -= camera->lookAt[1] * movementSpeed;
        camera->position[2] -= camera->lookAt[2] * movementSpeed;
    }
    if (keystrokes['A'])
    {
        // Left movement (relevant to the torso vector's perpendicular vector).
        camera->position[0] += cos_horz * movementSpeed;
        camera->position[2] -= sin_horz * movementSpeed;
    }
    if (keystrokes['D'])
    {
        // Right movement.
        camera->position[0] -= cos_horz * movementSpeed;
        camera->position[2] += sin_horz * movementSpeed;
    }
    if (keystrokes['X'])
    {
        // Downwards movement.
        camera->position[1] -= movementSpeed;
    }
    if (keystrokes[' '])
    {
        // Upwards movement.
        camera->position[1] += movementSpeed;
    }

    gluLookAt(
        (double)camera->position[0], 
        (double)camera->position[1], 
        (double)camera->position[2],

        (double)(camera->position[0] + camera->lookAt[0]), 
        (double)(camera->position[1] + camera->lookAt[1]), 
        (double)(camera->position[2] + camera->lookAt[2]),

        (double)camera->upVector[0], 
        (double)camera->upVector[1], 
        (double)camera->upVector[2]
    );
}

inline void deleteCamera(Camera* camera)
{
    if (camera != NULL)
        free(camera);
}

#endif // CAMERA_H