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
#include "MenuScreen.h"
#include "AmbientStars.h"
#include "TextRendering.h"
#include "MouseCallback.h"
#include "StellarObject.h"
#include "KeyboardCallback.h"
#include "MouseWheelCallback.h"
#include "PassiveMotionCallback.h"


int window_width;
int window_height;
int window_id;
bool fullscreen_enabled;

double framerate;

float simulation_speed;

clock_t refresh_ts;


StellarObject** stellarObjects;
int numStellarObjects;

StellarObject*** cachedAncestors;
int* numCachedAncestors;


Camera* camera;

AmbientStars* starsSkyBox;

MenuScreen* mainMenuScreen;
MenuScreen* planetMenuScreen;


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
    if (argc < 3)
    {
        fprintf(stderr, "Please specify the JSON file of the astronomical system's objects' data.\n");
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    glutMouseWheelFunc(callbackMouseWheel);
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
    
    camera->movementSpeed *= moveSpeedScaleFactor;
    updateCamera(camera);
    moveSpeedScaleFactor = 1.0f;

    renderMenuScreen(planetMenuScreen);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (keystrokes['+']) {
        simulation_speed *= 1.05f;
    }
    if (keystrokes['-']) {
        simulation_speed /= 1.05f;
    }

    for (int i = 0; i < numStellarObjects; ++i)
    {
        if (stellarObjects[i]->parent == NULL)
            continue;

        updateStellarObject(stellarObjects[i], simulation_speed);
        renderStellarObject(stellarObjects[i], true, cachedAncestors[i], &numCachedAncestors[i]);
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
        free(buffer);
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

    simulation_speed = 1.0f;

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
        .0f, 1.0f, .0f,
        100000.0f
    );

    // ----------- Stellar Objects (BEGIN) ----------- //

    stellarObjects = loadAllStellarObjects(&numStellarObjects, argv[2]);

    if (stellarObjects == NULL)
        exit(EXIT_FAILURE);

    cachedAncestors = (StellarObject ***)malloc(numStellarObjects * sizeof(StellarObject **));
    numCachedAncestors = (int *)malloc(numStellarObjects * sizeof(int));

    for (int i = 0; i < numStellarObjects; ++i)
    {
        cachedAncestors[i] = getStellarObjectAncestors(stellarObjects[i], &numCachedAncestors[i]);
        // printf("%s -> cached: %d\n", stellarObjects[i]->name, numCachedAncestors[i]);
    }
    // ----------- Stellar Objects (END) ----------- //


    mainMenuScreen = initMenuScreen(
        4,
        "Goofy",
        "Goofy 2",
        "kati tetoia",
        "Exit"
    );
    setMenuScreenBoxDimensions(mainMenuScreen, 0.12f, 0.04f);

    planetMenuScreen = initMenuScreenEmpty(numStellarObjects);

    for (int i = 0; i < numStellarObjects; ++i) {
        assignMenuScreenElement(planetMenuScreen, i, stellarObjects[i]->name);
    }
    setMenuScreenBoxDimensions(planetMenuScreen, 0.07f, 0.03f);

    starsSkyBox = buildStars(1000, camera);
}

// Free all dynamically allocated memory and FreeGLUT's resources.
void deallocateAll(void)
{
    for (int i = 0; i < numStellarObjects; ++i)
    {
        deleteStellarObject(stellarObjects[i]);
        
        if (cachedAncestors[i] != NULL)
            free(cachedAncestors[i]);
    }
    
    free(stellarObjects);
    free(cachedAncestors);
    free(numCachedAncestors);

    deleteCamera(camera);

    deleteStars(starsSkyBox);

    deleteMenuScreen(mainMenuScreen);
    deleteMenuScreen(planetMenuScreen);

    glutExit();
}
