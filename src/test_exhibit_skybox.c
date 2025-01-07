#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#   define _CRT_SECURE_NO_WARNINGS
#endif 

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#   define _USE_MATH_DEFINES
#endif 


#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include <GL/glut.h>

#include "Timer.h"
#include "Textures.h"
#include "CustomTypes.h"
#include "TextRendering.h"
#include "KeyboardCallback.h"


#define HELIX_RADIUS    4.0
#define VERTICAL_SPEED  2.0


vector3f cam_pos = {.0f, .0f, .0f};
vector3f cam_ori = {.0f, .0f, .0f};

unsigned int texture_id = 0;



void updateCamera(void)
{
    static double tau = -(M_PI);

    if (keystrokes['R'])
    {
        tau = -(M_PI);
    }

    static double c = VERTICAL_SPEED;

    double sin_tau = sin(tau) * HELIX_RADIUS;
    double cos_tau = cos(tau) * HELIX_RADIUS;

    cam_pos[0] = (float)cos_tau;
    cam_pos[1] = (float)(c * tau);
    cam_pos[2] = (float)sin_tau;

    cam_ori[0] = (float)(-sin_tau);
    cam_ori[1] = (float)c;
    cam_ori[2] = (float)cos_tau;

    // Normalization
    float len = vectorLength3fv(cam_ori);
    cam_ori[0] /= len;
    cam_ori[1] /= len;
    cam_ori[2] /= len;

    tau += (M_PI / 100.0);
}

void renderScene(bool has_texture)
{
    static GLUquadric* cameraBody = NULL;
    static GLUquadric* worldSkybox = NULL;
    static GLUquadric* worldSkyboxLattice = NULL;
    static GLUquadric* customStar = NULL;

    static vector3f star_pos[1000];
    static const int NUM_CUSTOM_STARS = sizeof(star_pos) / sizeof(star_pos[0]);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (cameraBody == NULL)
    {
        cameraBody = gluNewQuadric();
        gluQuadricDrawStyle(cameraBody, GLU_FILL);
    }

    if (worldSkybox == NULL)
    {
        worldSkybox = gluNewQuadric();
        gluQuadricDrawStyle(worldSkybox, GLU_FILL);
        gluQuadricTexture(worldSkybox, GL_TRUE);
    }

    if (worldSkyboxLattice == NULL)
    {
        worldSkyboxLattice = gluNewQuadric();
        gluQuadricDrawStyle(worldSkyboxLattice, GLU_SILHOUETTE);
    }

    if (customStar == NULL)
    {
        customStar = gluNewQuadric();
        gluQuadricDrawStyle(customStar, GLU_FILL);

        for (int i = 0; i < NUM_CUSTOM_STARS; ++i) 
        {
            double x = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
            double y = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
            double z = 2.0 * ((double)rand() / RAND_MAX) - 1.0;

            double len = sqrt(x * x + y * y + z * z);

            x /= len;
            y /= len;
            z /= len;

            star_pos[i][0] = (float)x;
            star_pos[i][1] = (float)y;
            star_pos[i][2] = (float)z;
        }
    }


    glPushMatrix();
    {
        glRotatef(-30.0f, .0f, .0f, 1.0f);

        glTranslatef(.0f, -20.0f, .0f);

        glTranslatef(
            cam_pos[0], 
            cam_pos[1], 
            cam_pos[2]
        );
        
        glColor4f(1.0f, .0f, .0f, 1.0f);

        glBegin(GL_LINES);
        {
            glVertex3f(.0f, .0f, .0f);
            glVertex3fv(cam_ori);
        }
        glEnd();


        // Render Camera
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        gluSphere(cameraBody, .4, 64, 32);

        static const float world_sphere_radius = 4.;

        glPushMatrix();
        {
            glRotatef(90.0f, 1.0f, .0f, .0f);

            if (has_texture)
            {
                // Render Textured Skybox
                glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                gluSphere(worldSkybox, world_sphere_radius, 64, 32);
                glDisable(GL_TEXTURE_2D);
            }
            else
            {
                // Render Custom Skybox
                glColor4f(1.0f, 1.0f, 1.0f, 0.3f);

                for (int i = 0; i < NUM_CUSTOM_STARS; ++i)
                {
                    glPushMatrix();

                    glScalef(
                        world_sphere_radius, 
                        world_sphere_radius, 
                        world_sphere_radius
                    );
                    glTranslatef(
                        star_pos[i][0],
                        star_pos[i][1],
                        star_pos[i][2]
                    );
                    gluSphere(customStar, .015, 3, 3);

                    glPopMatrix();
                }
            }

            // Render Skybox Lattice
            glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
            gluSphere(worldSkyboxLattice, 1.05 * world_sphere_radius, 16, 8);
        }
        glPopMatrix();


        glDisable(GL_DEPTH_TEST);

        renderStringInWorld(
            -2.4f, .2f, .0f,
            GLUT_BITMAP_9_BY_15, 
            "Camera",
            0xFF, 0xFF, 0xFF
        );
        
        glEnable(GL_DEPTH_TEST);
    }
    glPopMatrix();
    
    glPopMatrix();
}

void display(void)
{
    static bool has_texture = false;

    keyToggle('T', &has_texture, 250);

    if (keystrokes[27])
        exit(EXIT_SUCCESS);

    static uint64_t refresh_ts = 0;

    uint64_t current_ms = getAbsoluteTimeMillis();

    if (current_ms - refresh_ts < 5) 
    {
        glutPostRedisplay();
        return;
    }
    refresh_ts = current_ms;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0, 16.0 / 9.0, 0.1, 1000.0);
    gluLookAt(
        0.0, -10.0, 30.0,
        .0, .0, -1.0,
        .0, 1.0, .0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    updateCamera();
    renderScene(has_texture);

    glutSwapBuffers();
    glutPostRedisplay();
}


#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   720


int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++) {
        printf("[%d]: %s\n", i, argv[i]);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(
        (int)((WINDOW_HEIGHT / 4) * (16.0 / 9.0)),
        (int)((WINDOW_HEIGHT / 4))
    );
    glutCreateWindow("Implementation Exhibition - AmbientStars.h (Helix Trajectory)");

    initModuleKeyboardCallback();

    {
        char* bitmap_filepath = strCat(2, argv[2], "sample_universe.bmp");
        registerTexture(bitmap_filepath, &texture_id);
        free(bitmap_filepath);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);


    glutDisplayFunc(display);
    glutKeyboardFunc(callbackKeyboardDown);
    glutKeyboardUpFunc(callbackKeyboardUp);


    glutMainLoop();

    return EXIT_SUCCESS;
}
