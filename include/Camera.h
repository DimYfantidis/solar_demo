#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

#include "CustomTypes.h"
#include "StellarObject.h"
#include "KeyboardCallback.h"
#include "MotionCallback.h"


// Documentation:
// - IV. Interaction: https://github.com/DimYfantidis/solar_demo?tab=readme-ov-file#iv-interaction
// - V. Classes@Camera: https://github.com/DimYfantidis/solar_demo?tab=readme-ov-file#camera
typedef struct Camera
{
    // 3D coordinate vector of the camera's coordinates relative to the global coordinate system.
    vector3r position;

    // Unary orientation vector for specifying the camera's gaze direction.
    vector3r lookAt;

    // Necessary for `gluLookAt`.
    vector3r upVector;

    real_t movementSpeed;

    real_t renderDistance;

    // Non-null value implies the camera is in locked mode, i.e. observes the pointed astronomical 
    // object without freedom of movement (check "IV. Interaction" section in documentation).
    StellarObject* anchor;
    
} Camera;


// Camera constructor (heap-allocated).
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

    camera->anchor = NULL;

    return camera;
}


// Listens for events to update camera position and orientation, and modifies GLUT's projection matrix.
void updateCamera(Camera* camera)
{
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60.0, 16.0 / 9.0, 0.01, (double)camera->renderDistance);

    double sin_vert = sin((double)camera_angle_vertical);
	double sin_horz = sin((double)camera_angle_horizontal);
	double cos_vert = cos((double)camera_angle_vertical);
	double cos_horz = cos((double)camera_angle_horizontal);

    // Camera direction, defined by the unary sphere.
    camera->lookAt[0] = (real_t)(cos_vert * sin_horz);
    camera->lookAt[1] = (real_t)sin_vert;
    camera->lookAt[2] = (real_t)(cos_vert * cos_horz);

    real_t movementSpeed = camera->movementSpeed;


    if (camera->anchor != NULL)
    {
        camera->position[0] = camera->anchor->position[0];
        camera->position[1] = camera->anchor->position[1];
        camera->position[2] = camera->anchor->position[2];

        camera->position[0] += camera->anchor->radius * (real_t)4.5;
        camera->position[1] += camera->anchor->radius * (real_t)4.5;
        camera->position[2] += camera->anchor->radius * (real_t)4.5;

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

        return;
    }


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