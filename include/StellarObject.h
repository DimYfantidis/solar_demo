#ifndef STELLAR_OBJECT_H
#define STELLAR_OBJECT_H

#include <cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#include "Textures.h"
#include "CustomTypes.h"
#include "TextRendering.h"
#include "KeyboardCallback.h"


typedef struct StellarObject
{
    char* name;

    GLUquadric* quad;

    // The centre of the body's rotation.
    struct StellarObject* parent;

    vector3r position;

    real_t radius;

    // The body's linear velocity along its trajectory in rad/h.
    real_t angularVelocity;

    // The time period in which the body completes its orbit in solar days.
    real_t orbitalPeriod;

    // Value of the body's parametric equation of its trajectory. 
    real_t parametricAngle;

    // Distance from the body's centre of rotation, i.e. its trajectory's radius in AU.
    real_t parentDistance;

    // The trajectory's angle relevant to its parent's coordinate system.
    real_t solarTilt;
    // The trajectory's angle relevant to its parent's coordinate system.
    real_t globalSolarTilt;

    real_t sinGlobalSolarTilt;
    real_t cosGlobalSolarTilt;

    real_t selfAngularVelocity;

    real_t selfParametricAngle;

    vector3ub color;

    bool hasTexture;

    // The body's OpenGL texture ID
    GLuint texture; 

} StellarObject;


StellarObject* initStellarObject(
    const char* name, 
    real_t radius, 
    real_t orbit_period,
    StellarObject* parent, 
    real_t parent_dist,
    real_t solar_tilt,
    GLuint texture,
    bool has_texture,
    real_t day_period
) 
{
#ifdef PROJ_DEBUG
    printf("Creating StellarObject of radius %.2f\n", radius);
#endif
    StellarObject* p = (StellarObject*)malloc(sizeof(StellarObject));

    p->name = strBuild(name);

    p->radius = AUtoR(radius);

    p->parent = parent;

    p->solarTilt = solar_tilt;

    p->globalSolarTilt = solar_tilt + (parent != NULL ? parent->globalSolarTilt : (real_t).0);

    p->cosGlobalSolarTilt = (real_t)cos((double)p->globalSolarTilt * (M_PI / 180.0));
    p->sinGlobalSolarTilt = (real_t)sin((double)p->globalSolarTilt * (M_PI / 180.0));

    p->quad = gluNewQuadric();

    p->parametricAngle = -M_PI;

    p->selfParametricAngle = -M_PI;

    p->parentDistance = AUtoR(parent_dist);

    p->orbitalPeriod = orbit_period;

    p->texture = texture;

    p->hasTexture = has_texture;

    p->selfAngularVelocity = (real_t)1.0 / day_period;


    if (p->orbitalPeriod == (real_t).0)
    {
        fprintf(
            stderr, 
            "Warning: %s's orbitalPeriod was declared 0 - Proceeding with 0 angular velocity; Was this intentional?\n", 
            name
        );
        p->angularVelocity = (real_t).0;
    }
    else
    {
        p->angularVelocity = (real_t)(2.0 * M_PI / ((double)orbit_period * 24.0));
    }

    memset(p->color, (int)0xFF, sizeof(p->color));
    memset(p->position, (int).0, sizeof(p->position));

    if (has_texture)
        gluQuadricTexture(p->quad, GL_TRUE);

    gluQuadricDrawStyle(p->quad, GLU_FILL);

    return p;
}

StellarObject* coloriseStellarObject3f(StellarObject* p, float red, float green, float blue)
{
    p->color[0] = (ubyte)(red * 255);
    p->color[1] = (ubyte)(green * 255);
    p->color[2] = (ubyte)(blue * 255);
    
    return p;
}

StellarObject* coloriseStellarObject3ub(StellarObject* p, ubyte red, ubyte green, ubyte blue)
{
    p->color[0] = red;
    p->color[1] = green;
    p->color[2] = blue;
    
    return p;
}

void deleteStellarObject(StellarObject* p)
{
    if (p != NULL)
    {
        gluDeleteQuadric(p->quad);
        free(p->name);
        glDeleteTextures(1, &p->texture);
        free(p);
    }
}

void updateStellarObject(StellarObject* p, real_t speed_factor, real_t dt)
{
    p->parametricAngle += speed_factor * p->angularVelocity * dt;
    p->selfParametricAngle += speed_factor * p->selfAngularVelocity * dt;

    if (p->parametricAngle >= (real_t)M_PI) {
        p->parametricAngle -= (real_t)(2. * M_PI);
    }
    if (p->selfParametricAngle >= (real_t)M_PI) {
        p->selfParametricAngle -= (real_t)(2. * M_PI);
    }

    // Parametric position along its 2D (circular) trajectory.
    real_t parametricX = (real_t)(cos((double)p->parametricAngle) * (double)p->parentDistance);
    real_t parametricZ = (real_t)(sin((double)p->parametricAngle) * (double)p->parentDistance);

    // Apparent 3D position with respect to their parent's (rotated) coordinate system.
    p->position[0] = parametricX * p->cosGlobalSolarTilt;
    p->position[1] = parametricX * p->sinGlobalSolarTilt;
    p->position[2] = parametricZ;

    if (p->parent != NULL)
    {
        // Add parent's apparent coordinates to get 3D position with respect to the global coordinate system.
        p->position[0] += p->parent->position[0];
        p->position[1] += p->parent->position[1];
        p->position[2] += p->parent->position[2];
    }
}

// Returns an array of the body's system centre of rotation, along with 
// each subsequent system's centre of rotation, recursively. 
//
// E.g:
//  - Earth returns [The Sun]
//  - Venus returns [The Sun]
//  - Moon returns  [Earth, The Sun]
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

void renderStellarObject(
    StellarObject* p, 
    bool render_trajectory, 
    unsigned int trajectory_list_id
)
{
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glLoadIdentity();


    glTranslatef(
        (float)p->position[0],
        (float)p->position[1],
        (float)p->position[2]
    );

    if (p->hasTexture)
    {
        // Textured astronomical objects must be white so that their texture gets rendered properly.
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, p->texture);
    }
    else
    {
        glColor3ubv(p->color);
    }

    glPushMatrix();

    glRotatef((float)p->solarTilt - 90.0f, 1.0f, .0f, .0f);
    // Animation for rotation around axis.
    glRotatef((float)(p->selfParametricAngle * (real_t)(180.0 / M_PI)), .0f, .0f, 1.0f);
    // Render planet.
    gluSphere(p->quad, (double)p->radius, 64, 32);

    glPopMatrix();

    if (p->hasTexture)
    {
        glDisable(GL_TEXTURE_2D);
    }

    // Render planet's nametag
    renderStringInWorld(
        .0f, (float)(p->radius * (real_t)1.1), .0f,
        GLUT_BITMAP_9_BY_15, p->name,
        0xFF, 0xFF, 0xFF
    );

    glPopMatrix();

    glPushMatrix();

    if (render_trajectory && p->parent != NULL)
    {
        glColor4ub(p->color[0], p->color[1], p->color[2], 38);

        glTranslatef(
            (float)p->parent->position[0],
            (float)p->parent->position[1],
            (float)p->parent->position[2]
        );

        glRotatef((float)p->globalSolarTilt, .0f, .0f, 1.0f);

        glScalef(
            (float)p->parentDistance, 
            (float)p->parentDistance, 
            (float)p->parentDistance
        );

        glCallList(trajectory_list_id);
    }
    glPopMatrix();
}

unsigned int generateStellarObjectTrajectoryDisplayList()
{
    unsigned int listId = glGenLists(1);

    glNewList(listId, GL_COMPILE);
    {
        glBegin(GL_LINE_LOOP);
        {
            for (float theta = (float)(-M_PI); theta < (float)M_PI; theta += (float)M_PI / 3000.0f)
                glVertex3f(cosf(theta), .0f, sinf(theta));
        }
        glEnd();
    }
    glEndList();

    return listId;
}

StellarObject** loadAllStellarObjects(int* arraySize, const char* data_dir)
{
    StellarObject** destArray;

    *arraySize = 0;

    char* json_filename = strCat(2, data_dir, "data.json");

    // open the JSON file 
    FILE *fp = fopen(json_filename, "r"); 

    if (fp == NULL) 
    { 
        fprintf(stderr, "Error: Unable to open the JSON file \"%s\".\n", json_filename); 
        return NULL;
    } 

    // read the file contents into a string 
    const size_t JSON_BUFFER_SIZE = 1024 * 1024;

    char* buffer = (char *)malloc(JSON_BUFFER_SIZE * sizeof(char));

    size_t len = fread(buffer, sizeof(char), JSON_BUFFER_SIZE, fp);

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
        return NULL; 
    }

    const cJSON *iterator = NULL;
    const cJSON *allStellarObjects = NULL;

    allStellarObjects = cJSON_GetObjectItemCaseSensitive(json, "Astronomical Objects");

    if (!cJSON_IsArray(allStellarObjects))
    {
        fprintf(
            stderr, 
            "Error: %s should be an array of objects; Inspect \"%s\"", 
            allStellarObjects->string, json_filename
        );
        return NULL;
    }

    destArray = (StellarObject **)malloc(cJSON_GetArraySize(allStellarObjects) * sizeof(StellarObject *));

    cJSON_ArrayForEach(iterator, allStellarObjects)
    {
        if (!cJSON_IsObject(iterator))
        {
            fprintf(
                stderr,
                "Error: %s should only contain JSON objects; Inspect arrray item idx.#%d in \"%s\"", 
                allStellarObjects->string, *arraySize, json_filename
            );

            for (int i = 0; i < *arraySize; ++i)
                deleteStellarObject(destArray[i]);

            free(destArray);
            
            return NULL;
        }

        static const char* errorFieldMessage = "Error: JSON array idx.#%d - StellarObject's `%s` field is invalid; Inspect \"%s\".\n";
        char* errorField = NULL;


        cJSON *name = cJSON_GetObjectItemCaseSensitive(iterator, "name");
        cJSON *radius = cJSON_GetObjectItemCaseSensitive(iterator, "radius");
        cJSON *orbit_period = cJSON_GetObjectItemCaseSensitive(iterator, "orbit_period");
        cJSON *parent = cJSON_GetObjectItemCaseSensitive(iterator, "parent");
        cJSON *parent_dist = cJSON_GetObjectItemCaseSensitive(iterator, "parent_dist");
        cJSON *solar_tilt = cJSON_GetObjectItemCaseSensitive(iterator, "solar_tilt");
        cJSON *color = cJSON_GetObjectItemCaseSensitive(iterator, "color");
        cJSON *day_period = cJSON_GetObjectItemCaseSensitive(iterator, "day_period");


        if (!cJSON_IsString(name) || name->valuestring == NULL)
            errorField = strBuild("name");

        else if (!cJSON_IsNumber(radius))
            errorField = strBuild("radius");

        else if (!cJSON_IsNumber(orbit_period) && !cJSON_IsNull(orbit_period))
            errorField = strBuild("orbit_period");

        else if (!cJSON_IsString(parent) && !cJSON_IsNull(parent))
            errorField = strBuild("parent");

        else if (!cJSON_IsNumber(parent_dist) && !cJSON_IsNull(parent_dist))
            errorField = strBuild("parent_dist");

        else if (!cJSON_IsNumber(solar_tilt))
            errorField = strBuild("solar_tilt");

        else if (!cJSON_IsArray(color) || (cJSON_GetArraySize(color) != 3))
            errorField = strBuild("color");

        else if (!cJSON_IsNumber(day_period))
            errorField = strBuild("day_period");


        if (errorField != NULL)
        {
            fprintf(stderr, errorFieldMessage, *arraySize, errorField, json_filename);
            free(destArray);
            free(errorField);
            return NULL;
        }

        if (!cJSON_IsNull(parent) && (!cJSON_IsNumber(parent_dist) || !cJSON_IsNumber(orbit_period)))
        {
            fprintf(
                stderr, 
                "Error: %s's parent was declared non-null but dependent fields are invalid; Inspect \"%s\".\n",
                name->valuestring, 
                json_filename
            );
            free(destArray);
            free(errorField);
            return NULL;
        }

        StellarObject* parentRaw = NULL;

        if (!cJSON_IsNull(parent))
        {
            int i;

            for (i = 0; i < *arraySize; ++i)
            {
                if (strcmp(parent->valuestring, destArray[i]->name) != 0)
                    continue;
                
                parentRaw = destArray[i];
                break;
            }

            if (i == *arraySize)
            {
                fprintf(
                    stderr,
                    "Warning: %s was declared as %s's planetary anchor but this StellarObject is not found and will thus not render.\n"
                    "         Inspect JSON file \"%s\"\n"
                    "         If %s does exist within the file, reorder it so that it's above %s\n",
                    parent->valuestring,
                    name->valuestring,
                    json_filename,
                    parent->valuestring,
                    name->valuestring
                );
            }
        }

        real_t orbitPeriodRaw = (cJSON_IsNull(orbit_period) ? (real_t)1.0 : (real_t)orbit_period->valuedouble);
        real_t parentDistanceRaw = (cJSON_IsNull(parent_dist) ? (real_t)0.0 : (real_t)parent_dist->valuedouble);

        cJSON* r = cJSON_GetArrayItem(color, 0);
        cJSON* g = cJSON_GetArrayItem(color, 1);
        cJSON* b = cJSON_GetArrayItem(color, 2);


        GLuint textureId;

        char* texture_filename = strCat(3, data_dir, name->valuestring, ".bmp");

        bool has_texture = registerTexture(texture_filename, &textureId);

        // Load Textures
        if (!has_texture)
        {
            fprintf(
                stderr, 
                "Warning: Could not load texture for %s; Continuing with `glColor*`.\n", 
                name->valuestring
            );
        }

        free(texture_filename);

        destArray[*arraySize] = (

            coloriseStellarObject3ub(

                initStellarObject(
                    name->valuestring,
                    (real_t)radius->valuedouble,
                    orbitPeriodRaw,
                    parentRaw,
                    parentDistanceRaw,
                    (real_t)solar_tilt->valuedouble,
                    textureId,
                    has_texture,
                    (real_t)day_period->valuedouble
                ),

                (ubyte)r->valueint, (ubyte)g->valueint, (ubyte)b->valueint
            )
        );

        *arraySize += 1;
    }


    cJSON_Delete(json);

    free(buffer);

    free(json_filename);

    return destArray;
}

#endif // STELLAR_OBJECT_H