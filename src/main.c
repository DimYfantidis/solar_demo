#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#   define _CRT_SECURE_NO_WARNINGS
#endif 

#if defined(_MSC_VER) && !defined(_USE_MATH_DEFINES)
#   define _USE_MATH_DEFINES
#endif 

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <cJSON.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include "Timer.h"
#include "Camera.h"
#include "MenuScreen.h"
#include "CustomTypes.h"
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

real_t simulationSpeed;

uint64_t refresh_ts;

StellarObject** stellarObjects;

int numStellarObjects;

unsigned int trajectoryListId; 

StellarObject*** cachedAncestors;
int* numCachedAncestors;

Camera* camera;

AmbientStars* starsSkyBox;

bool skyTexture;

bool enableHUD;
bool enablePlanetMenu;
bool enableMainMenu;

MenuScreen* mainMenuScreen;
MenuScreen* planetMenuScreen;

uint64_t simulationElapsedTimeMilliseconds;
uint64_t realElapsedTimeMilliseconds;


void initGlobals(int, char**);
void deallocateAll(void);
void display(void);

int main(int argc, char* argv[])
{
    // argv[1] should be the filepath of "_constants.json" (found within "./data" dir)
    if (argc < 2)
    {
        fprintf(stderr, "Please specify the JSON file of the dynamically loaded constants.\n");
        return EXIT_FAILURE;
    }
    // argv[2] should be the filepath of the astronomical system's data. 
    if (argc < 3)
    {
        fprintf(stderr, "Please specify the JSON file of the astronomical system's objects' data.\n");
        return EXIT_FAILURE;
    }
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(1280, 720);
	window_id = glutCreateWindow("Solar System - exhibition");

    initGlobals(argc, argv); 

    glutReshapeWindow(window_width, window_height);

    if (fullscreen_enabled)
        glutFullScreen();

	glClearColor(
        0.0196078431372549f / 4, 
        0.029411764705882353f / 3, 
        0.0803921568627451f / 3, 
        1.0f
    );

	glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    printf("[1] >>> Hello, Universe!\n");

    glutSetCursor(GLUT_CURSOR_NONE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    // Initialise the callback function's static variables with the provided arguments.
    callbackPassiveMotion(windowCentreX, windowCentreY);
    callbackPassiveMotion(windowCentreX, windowCentreY);

    glutDisplayFunc(display);
    glutKeyboardFunc(callbackKeyboard);
    glutKeyboardUpFunc(callbackKeyboardUp);
    glutSpecialFunc(callbackSpecialKeyboard);
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

    return EXIT_SUCCESS;
}




void display(void)
{
    keyToggle('H', &enableHUD, 250);
    keyToggle('P', &enablePlanetMenu, 250);
    keyToggle(27,  &enableMainMenu, 250);

    double elapsed_seconds = (double)(getAbsoluteTimeMillis() - refresh_ts) / 1000.0;

    // Force framerate cap using time scheduling variables. 
    if (elapsed_seconds < 1.0 / framerate) 
    {
        glutPostRedisplay();
        return;
    }
    refresh_ts = getAbsoluteTimeMillis();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    camera->movementSpeed *= moveSpeedScaleFactor;
    updateCamera(camera);
    moveSpeedScaleFactor = 1.0f;


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (keystrokes['+']) {
        simulationSpeed *= (real_t)1.05;
    }
    if (keystrokes['-']) {
        simulationSpeed /= (real_t)1.05;
    }

    for (int i = 0; i < numStellarObjects; ++i)
    {
        // Update celestial body's position after moving 
        // by v * dt, where v is its linear velocity.
        updateStellarObject(stellarObjects[i], simulationSpeed, (float)elapsed_seconds / 3600.0f);
        // Render the body as well as its trajectory.
        renderStellarObject(stellarObjects[i], true, trajectoryListId);
    }

    camera->movementSpeed *= moveSpeedScaleFactor;
    updateCamera(camera);
    moveSpeedScaleFactor = 1.0f;

    const char* option;
    int optionIndex;

    if (enableMainMenu)
    {
        enablePlanetMenu = false;

        renderMenuScreen(mainMenuScreen);

        if ((option = menuScreenHandler(mainMenuScreen, NULL)) != NULL)
        {
            if (strcmp(option, "Free-fly") == 0)
                camera->anchor = NULL;
            
            else if (strcmp(option, "Help") == 0)
            {
                static const char help_page[] = "https://github.com/DimYfantidis/solar_demo?tab=readme-ov-file#iv-interaction";

                printf("Opening Help web-page: %s\n", help_page);

                int status = openBrowserAt(help_page);

                if (status != EXIT_SUCCESS)
                    fprintf(
                        stderr, 
                        "Error: Could not open web-browser to the Help page; Please proceed to \"%s\" manually", 
                        help_page
                    );
            }

            else if (strcmp(option, "Exit") == 0)
            {
                glutDestroyWindow(window_id);
                glutLeaveMainLoop();
                return;
            }

            enableMainMenu = false;
        }
    }
    if (enablePlanetMenu)
    {
        renderMenuScreen(planetMenuScreen);

        if ((option = menuScreenHandler(planetMenuScreen, &optionIndex)) != NULL)
        {
            camera->anchor = stellarObjects[optionIndex];
            enablePlanetMenu = false;
        }
    }

    renderStars(starsSkyBox);


    static char timeFormatBuffer[1024];

    simulationElapsedTimeMilliseconds += (uint64_t)(elapsed_seconds * simulationSpeed * 1000);
    realElapsedTimeMilliseconds += (uint64_t)(elapsed_seconds * 1000);

    static char logBuffer[1024];
    
    if (enableHUD)
    {
        snprintf(logBuffer, sizeof(logBuffer), "FPS: %.2lf", 1 / elapsed_seconds);
        renderStringOnScreen(0.0, window_height - 15.0f, GLUT_BITMAP_9_BY_15, logBuffer, 0xFF, 0xFF, 0xFF);

        snprintf(logBuffer, sizeof(logBuffer), "Camera Position: (%lf, %lf, %lf)", camera->position[0], camera->position[1], camera->position[2]);
        renderStringOnScreen(0.0, window_height - 30.0f, GLUT_BITMAP_9_BY_15, logBuffer, 0xFF, 0xFF, 0xFF);

        snprintf(logBuffer, sizeof(logBuffer), "Simulation Speed: %.4f", simulationSpeed);
        renderStringOnScreen(0.0, window_height - 45.0f, GLUT_BITMAP_9_BY_15, logBuffer, 0xFF, 0xFF, 0xFF);

        snprintf(logBuffer, sizeof(logBuffer), "Camera Speed: %.4f", camera->movementSpeed);
        renderStringOnScreen(0.0, window_height - 60.0f, GLUT_BITMAP_9_BY_15, logBuffer, 0xFF, 0xFF, 0xFF);

        getTimeFormatStringFromMillis(timeFormatBuffer, sizeof(timeFormatBuffer), realElapsedTimeMilliseconds);
        snprintf(logBuffer, sizeof(logBuffer), "Elapsed Real time:    %s", timeFormatBuffer);
        renderStringOnScreen(0.0, window_height - 90.0f, GLUT_BITMAP_9_BY_15, logBuffer, 0xFF, 0xFF, 0xFF);

        getTimeFormatStringFromMillis(timeFormatBuffer, sizeof(timeFormatBuffer), simulationElapsedTimeMilliseconds);
        snprintf(logBuffer, sizeof(logBuffer), "Elapsed Virtual time: %s", timeFormatBuffer);
        renderStringOnScreen(0.0, window_height - 105.0f, GLUT_BITMAP_9_BY_15, logBuffer, 0xFF, 0xFF, 0xFF);
    }

    float pixelOffsetCentre;

    if (camera->anchor != NULL)
    {
        snprintf(logBuffer, sizeof(logBuffer), "Press ESC -> \"Free-Fly\" -> ENTER to stop observing %s", camera->anchor->name);
        pixelOffsetCentre = (float)strlen(logBuffer) / 2.0f;
        renderStringOnScreen(0.50f * window_width - pixelOffsetCentre * 9.0f, 0.20f * window_height, GLUT_BITMAP_9_BY_15, logBuffer, 0xFF, 0xFF, 0xFF);

        renderStringOnScreen(
            0.50f * window_width - 220.5f, 0.20f * window_height - 20.0f, 
            GLUT_BITMAP_9_BY_15, 
            "or press 'P' and choose another planet to observe", 
            0xFF, 0xFF, 0xFF
        );
    }

    glutSwapBuffers();
    glutPostRedisplay();
}


void initGlobals(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));

    // Default Values
    window_width = 1280;
    window_height = 720;
    framerate = 60.0;

    simulationElapsedTimeMilliseconds = 0;
    realElapsedTimeMilliseconds = 0;

    skyTexture = false;

    enableHUD = false;
    enablePlanetMenu = false;
    enableMainMenu = false;

    refresh_ts = getAbsoluteTimeMillis();

    // open the _constants.json file 
    FILE *fp = fopen(argv[1], "r"); 

    if (fp == NULL) 
    { 
        fprintf(stderr, "Error: Unable to open the JSON file.\n"); 
        exit(EXIT_FAILURE);
    }

    // Read the file contents into a string 
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

    
    cJSON *sky_texture = cJSON_GetObjectItemCaseSensitive(json, "sky_texture"); 
    if (cJSON_IsBool(sky_texture))
        skyTexture = (bool)sky_texture->valueint;


    // delete the JSON object 
    cJSON_Delete(json); 
    free(buffer);

    // Auxiliary variables for centering the mouse pointer using glutWarpPointer().
    windowCentreX = window_width / 2;
    windowCentreY = window_height / 2;

    // Independent variables for the parametric equation of a sphere.
    // Used for controlling the camera's orientation through mouse movement.
    CameraAngleHorizontal = .0f;
    CameraAngleVertical = .0f;

    mouseSensitivity = .002f;

    simulationSpeed = 1.0f;

    // Used for keyboard input (see KeyboardCallback.h)
    for (int i = 0; i < sizeof(keystrokes) / sizeof(keystrokes[0]); ++i) {
        keystrokes[i] = false;
    }

    camera = initCamera(
        // Initial camera position .
        16.47074, 32.79276,  5.98598,
        // Initial camera orientation.
        -0.444, -0.881, -0.163,
        // Up vector.
        .0, 1.0, .0,
        // Render distance in world units.
        20000.0
    );

    // ----------- Stellar Objects (BEGIN) ----------- //

    stellarObjects = loadAllStellarObjects(&numStellarObjects, argv[2]);

    if (stellarObjects == NULL)
        exit(EXIT_FAILURE);

    cachedAncestors = (StellarObject ***)malloc(numStellarObjects * sizeof(StellarObject **));
    // Number of ancestors for each celestial body. 
    numCachedAncestors = (int *)malloc(numStellarObjects * sizeof(int));

    for (int i = 0; i < numStellarObjects; ++i)
    {
        cachedAncestors[i] = getStellarObjectAncestors(stellarObjects[i], &numCachedAncestors[i]);
        // printf("%s -> cached: %d\n", stellarObjects[i]->name, numCachedAncestors[i]);
    }

    trajectoryListId = generateStellarObjectTrajectoryDisplayList();

    // ----------- Stellar Objects (END) ----------- //
    

    // ----------- Window Matrix (BEGIN) ----------- //
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Initialization of Window Matrix for on-screen string rendering.
	glPushMatrix();
	{
		gluOrtho2D(0.0, (double)window_width, 0.0, (double)window_height);
		glGetFloatv(GL_PROJECTION_MATRIX, windowMatrix);
	}
	glPopMatrix();
	// ----------- Window Matrix (BEGIN) ----------- //


    mainMenuScreen = setMenuScreenDimensions(
        initMenuScreen(
            windowMatrix,
            3,
            "Free-fly",
            "Help",
            "Exit"
        ),
        window_width, window_height
    );
    //setMenuScreenBoxDimensions(mainMenuScreen, 0.12f, 0.04f);

    planetMenuScreen = setMenuScreenDimensions(
        initMenuScreenEmpty(
            windowMatrix, 
            numStellarObjects
        ),
        window_width, window_height
    );

    for (int i = 0; i < numStellarObjects; ++i) {
        assignMenuScreenElement(planetMenuScreen, i, stellarObjects[i]->name);
    }
    //setMenuScreenBoxDimensions(planetMenuScreen, 0.07f, 0.03f);

    if (skyTexture)
        starsSkyBox = buildStarsFromTexture(argv[2], camera);
    else
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
