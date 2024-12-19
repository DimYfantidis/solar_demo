// #define PROJ_DEBUG

#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif 

#ifndef _USE_MATH_DEFINES
#   define _USE_MATH_DEFINES
#endif 

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <cJSON.h>

#include "Timer.h"
#include "Camera.h"
#include "AmbientStars.h"
#include "TextRendering.h"
#include "MouseCallback.h"
#include "StellarObject.h"
#include "KeyboardCallback.h"
#include "PassiveMotionCallback.h"


int window_width;
int window_height;
int window_id;
bool fullscreen_enabled;

double framerate;

clock_t refresh_ts;

StellarObject* world_centre_placeholder_ignorevar__;
StellarObject* sun;
StellarObject* earth;
StellarObject* moon;
StellarObject* mercury;
StellarObject* venus;
StellarObject* mars;
StellarObject* jupiter;
StellarObject* saturn;
StellarObject* uranus;
StellarObject* neptune;

StellarObject* stellarObjects[10];

Camera* camera;

AmbientStars* starsSkyBox;


void windowControl(void);
void initGlobals(int, char**);
void deallocateAll(void);
void display(void);

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Please specify the JSON file of the dynamically loaded constants.\n");
        return EXIT_FAILURE;
    }

	glutInit(&argc, argv);
    initGlobals(argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	window_id = glutCreateWindow("Solar System - exhibition");

    if (fullscreen_enabled)
        glutFullScreen();

	glClearColor(
        0.0196078431372549f / 4, 
        0.029411764705882353f / 3, 
        0.0803921568627451f / 3, 
        1.0f
    );
	glEnable(GL_DEPTH_TEST);

    printf("[1] >>> Hello, Universe!\n");

    glutSetCursor(GLUT_CURSOR_NONE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    callbackPassiveMotion(windowCentreX, windowCentreY);
    callbackPassiveMotion(windowCentreX, windowCentreY);

    glutDisplayFunc(display);
    glutKeyboardFunc(callbackKeyboard);
    glutKeyboardUpFunc(callbackKeyboardUp);
    glutMouseFunc(callbackMouse);
    glutMotionFunc(callbackPassiveMotion);
    glutPassiveMotionFunc(callbackPassiveMotion);


    {
        Timer* programTimer = initTimer("glutMainLoop");
	    glutMainLoop();
        endTimer(programTimer);
    }


    printf("[2] >>> Exited Main Loop.\n");

    deallocateAll();

    printf("[3] >>> Deallocated all memory.\n");

    return 0;
}




void display(void)
{
    if (keystrokes[27])
    {
        glutDestroyWindow(window_id);
        glutLeaveMainLoop();
        return;
    }

    double elapsed_seconds = (double)(clock() - refresh_ts) / CLOCKS_PER_SEC;

    if (elapsed_seconds < 1.0 / framerate) 
    {
        glutPostRedisplay();
        return;
    }
    refresh_ts = clock();
    
#ifdef PROJ_DEBUG
    printf("Clearing the screen (elapsed: %.3f sec)\n", elapsed_seconds);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    updateCamera(camera);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (int i = 0; i < sizeof(stellarObjects) / sizeof(stellarObjects[i]); ++i)
    {
        updateStellarObject(stellarObjects[i]);
        renderStellarObject(stellarObjects[i]);
    }

    renderStars(starsSkyBox);

    glutSwapBuffers();
    glutPostRedisplay();
}


void initGlobals(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));

    window_width = 1280;
    window_height = 720;

    framerate = 60.0;

    // open the JSON file 
    FILE *fp = fopen(argv[1], "r"); 

    if (fp == NULL) 
    { 
        fprintf(stderr, "Error: Unable to open the JSON file.\n"); 
        exit(EXIT_FAILURE);
    } 
  
    // read the file contents into a string 
    const size_t JSON_BUFFER_SIZE = 1024 * 1024;

    char* buffer = (char *)malloc(JSON_BUFFER_SIZE * sizeof(char));

    size_t len = fread(buffer, 1, JSON_BUFFER_SIZE, fp);

    fclose(fp); 
  
    // parse the JSON data 
    cJSON *json = cJSON_Parse(buffer); 

    if (json == NULL) 
    { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            fprintf(stderr, "Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        exit(EXIT_FAILURE); 
    }

    // Access the JSON window dimension data 
    cJSON *window_dimensions = cJSON_GetObjectItemCaseSensitive(json, "window_dimensions");
    if (cJSON_IsObject(window_dimensions) && (window_dimensions->string != NULL)) 
    { 
        cJSON *width = cJSON_GetObjectItemCaseSensitive(window_dimensions, "width");
        cJSON *height = cJSON_GetObjectItemCaseSensitive(window_dimensions, "height");

        if (cJSON_IsNumber(width))
            window_width = width->valueint;

        if (cJSON_IsNumber(height))
            window_height = height->valueint;
    }
    // Access the JSON fullscreen boolean data 
    cJSON *fullscreen = cJSON_GetObjectItemCaseSensitive(json, "fullscreen");
    if (cJSON_IsBool(fullscreen)) 
        fullscreen_enabled = (bool)fullscreen->valueint;

    // Access the JSON framerate data 
    cJSON *fps = cJSON_GetObjectItemCaseSensitive(json, "framerate"); 
    if (cJSON_IsNumber(fps))
        framerate = fps->valuedouble;

    // delete the JSON object 
    cJSON_Delete(json); 
    free(buffer);

    windowCentreX = window_width / 2;
    windowCentreY = window_height / 2;

    CameraAngleHorizontal = .0f;
    CameraAngleVertical = .0f;

    mouseSensitivity = .002f;

    for (int i = 0; i < sizeof(keystrokes) / sizeof(keystrokes[0]); ++i) {
        keystrokes[i] = false;
    }


    // ----------- Window Matrix (BEGIN) ----------- //
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPushMatrix();
	{
	    // Initialization of Window Matrix for on-screen string rendering.
		gluOrtho2D(0.0, (double)window_width, 0.0, (double)window_height);
		glGetFloatv(GL_PROJECTION_MATRIX, windowMatrix);
	}
	glPopMatrix();
    // ----------- Window Matrix (END) ----------- //

    camera = initCamera(
        16.47074f, 32.79276f,  5.98598f,
        -0.444f, -0.881f, -0.163f,
        .0f, 1.0f, .0f
    );
    
    // ----------- Stellar Objects (BEGIN) ----------- //
    world_centre_placeholder_ignorevar__ = initStellarObject(
        "[[WORLD_ORIGIN_DUMMY_IGNOREVAR]]", 
        .0f,
        .0f,
        NULL, 
        .0f,
        .0f
    ),
    stellarObjects[0] = sun = colorise3ub(
        initStellarObject(
            "The Sun", 
            AUtoR(0.00465474539672041f), 
            .0f,
            world_centre_placeholder_ignorevar__, 
            .0f,
            .0f
        ),
        0xFF, 0x4D, 0x00
    );
    stellarObjects[1] = earth = colorise3ub(
        initStellarObject(
            "Earth", 
            AUtoR(4.258750455597227e-05f),
            AUtoR(1.990990905E-7f),
            sun, 
            AUtoR(1.0f),
            10.0f
        ),
        0x00, 0x00, 0xFF
    );
    stellarObjects[2] = moon = colorise3ub(
        initStellarObject(
            "Moon", 
            AUtoR(0.0000116312f), 
            AUtoR(6.831648039E-9f),
            earth, 
            AUtoR(0.0025695553f),
            .0f
        ),
        0xFF, 0xFF, 0xFF
    );
    stellarObjects[3] = mercury = colorise3ub(
        initStellarObject(
            "Mercury", 
            AUtoR(1.6310392578335008e-05f), 
            AUtoR(3.2687631027892703e-07f),
            sun, 
            AUtoR(0.38f),
            -30.0f
        ),
        0xFA, 0xFA, 0xFA
    );
    stellarObjects[4] = venus = colorise3ub(
        initStellarObject(
            "Venus", 
            AUtoR(4.045512126396864e-05f), 
            AUtoR(2.327573235973873e-07f),
            sun, 
            AUtoR(0.73f),
            -10.0f
        ),
        0xFF, 0xE8, 0x93
    );
    stellarObjects[5] = mars = colorise3ub(
        initStellarObject(
            "Mars", 
            AUtoR(2.2660750344490033e-05f), 
            AUtoR(1.5695410563086313e-07f),
            sun, 
            AUtoR(1.57f),
            20.0f
        ),
        0x8A, 0x33, 0x24
    );
    stellarObjects[6] = jupiter = colorise3ub(
        initStellarObject(
            "Jupiter", 
            AUtoR(0.00046732617030490934f), 
            AUtoR(8.964031330961987e-08f),
            sun, 
            AUtoR(5.07f),
            40.0f
        ),
        0xF5, 0xEA, 0xB9
    );
    stellarObjects[7] = saturn = colorise3ub(
        initStellarObject(
            "Saturn", 
            AUtoR(0.0003892568773039362f), 
            AUtoR(6.377096114644097e-08f),
            sun, 
            AUtoR(9.64f),
            80.0f
        ),
        0xF5, 0xEA, 0xB9
    );
    stellarObjects[8] = uranus = colorise3ub(
        initStellarObject(
            "Uranus", 
            AUtoR(0.00016953449859497235f), 
            AUtoR(4.458619610553054e-08f),
            sun, 
            AUtoR(19.56f),
            50.0f
        ),
        0xCA, 0xE9, 0xF5
    );
    stellarObjects[9] = neptune = colorise3ub(
        initStellarObject(
            "Neptune", 
            AUtoR(0.0001645879041244937f), 
            AUtoR(3.649784568758572e-08f),
            sun, 
            AUtoR(29.9f),
            -42.0f
        ),
        0x84, 0xAC, 0xFA
    );
    // ----------- Stellar Objects (END) ----------- //

    starsSkyBox = buildStars(500, camera);
}

// Free all dynamically allocated memory and FreeGLUT's resources.
void deallocateAll(void)
{
    for (int i = 0; i < sizeof(stellarObjects) / sizeof(stellarObjects[0]); ++i)
    {
        deleteStellarObject(stellarObjects[i]);
    }
    deleteStellarObject(world_centre_placeholder_ignorevar__);

    deleteCamera(camera);

    deleteStars(starsSkyBox);

    glutExit();
}
