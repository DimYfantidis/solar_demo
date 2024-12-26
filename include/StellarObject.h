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

    vector3f position;

    float radius;

    // The body's linear velocity along its trajectory in rad/h.
    float angularVelocity;

    // The time period in which the body completes its orbit in solar days.
    float orbitalPeriod;

    // Value of the body's parametric equation of its trajectory. 
    float parametricAngle;

    // Distance from the body's centre of rotation, i.e. its trajectory's radius in AU.
    float parentDistance;

    // The trajectory's angle relevant to the parent's coordinate system.
    float solarTilt;

    float selfAngularVelocity;

    float selfParametricAngle;

    vector3ub color;

    bool hasTexture;

    // The body's OpenGL texture ID
    GLuint texture; 

} StellarObject;


StellarObject* initStellarObject(
    const char* name, 
    float radius, 
    float orbit_period,
    StellarObject* parent, 
    float parent_dist,
    float solar_tilt,
    GLuint texture,
    bool has_texture,
    float day_period
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
    p->quad = gluNewQuadric();
    p->parametricAngle = (float)(-M_PI);
    p->selfParametricAngle = (float)(-M_PI);
    p->parentDistance = AUtoR(parent_dist);
    p->orbitalPeriod = orbit_period;
    p->texture = texture;
    p->hasTexture = has_texture;
    p->selfAngularVelocity = 1.0f / day_period;

    if (p->orbitalPeriod == .0f)
    {
        fprintf(
            stderr, 
            "Warning: %s's orbitalPeriod was declared 0 - Proceeding with 0 angular velocity; Was this intentional?\n", 
            name
        );
        p->angularVelocity = .0f;
    }
    else
    {
        p->angularVelocity = (float)(2.0 * M_PI / ((double)orbit_period * 24.0));
    }

    memset(p->position, (int).0f, sizeof(p->position));
    memset(p->color, (int)0xFF, sizeof(p->color));

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
    // Textured astronomical objects must be white so that their texture gets rendered properly.
    if (!p->hasTexture)
    {
        p->color[0] = red;
        p->color[1] = green;
        p->color[2] = blue;
    }
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

void updateStellarObject(StellarObject* p, float speed_factor, float dt)
{
    p->parametricAngle += speed_factor * p->angularVelocity * dt;
    p->selfParametricAngle += speed_factor * p->selfAngularVelocity * dt;

    if (p->parametricAngle > (float)M_PI) {
        p->parametricAngle -= (float)(2. * M_PI);
    }
    if (p->selfParametricAngle > (float)M_PI) {
        p->selfParametricAngle -= (float)(2. * M_PI);
    }

    p->position[0] = cosf(p->parametricAngle) * p->parentDistance;
    p->position[2] = sinf(p->parametricAngle) * p->parentDistance;
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
    unsigned int trajectory_list, 
    StellarObject** ancestors, 
    int* n_ancestors
)
{
    bool usesCachedAncestors = (ancestors == NULL ? false : true);

    int tmp_n_ancestors;


    glColor3ubv(p->color);

    glPushMatrix();

    if (n_ancestors == NULL)
        n_ancestors = &tmp_n_ancestors;

    if (!usesCachedAncestors)
        ancestors = getStellarObjectAncestors(p, n_ancestors);

    // Apply the transformations of all previous astrological systems' centres of rotation. 
    for (int i = *n_ancestors - 1; i >= 0; --i) 
    {
        glRotatef(ancestors[i]->solarTilt, 0, 0, 1);

        glTranslatef(
            ancestors[i]->position[0],
            ancestors[i]->position[1],
            ancestors[i]->position[2]
        );
    }

    if (!usesCachedAncestors)
        free(ancestors);

    glRotatef(p->solarTilt, 0, 0, 1);

    // The above transformations are saved as they will be 
    // used for rendering the StellarObject's trajectory as well.
    glPushMatrix();

    glTranslatef(
        p->position[0], 
        p->position[1], 
        p->position[2]
    );

    if (p->hasTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, p->texture);
    }

    glPushMatrix();

    glRotatef(p->solarTilt - 90.0f, 1.0f, .0f, .0f);
    // Animation for rotation around axis.
    if (p->hasTexture)
        glRotatef(p->selfParametricAngle * (float)(180.0 / M_PI), .0f, .0f, 1.0f);
    // Render planet.
    gluSphere(p->quad, p->radius, 64, 32);

    glPopMatrix();

    if (p->hasTexture)
    {
        glDisable(GL_TEXTURE_2D);
    }

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

        glScalef(p->parentDistance, p->parentDistance, p->parentDistance);

        glCallList(trajectory_list);
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

        if (!cJSON_IsNumber(radius))
            errorField = strBuild("radius");

        if (!cJSON_IsNumber(orbit_period) && !cJSON_IsNull(orbit_period))
            errorField = strBuild("orbit_period");

        if (!cJSON_IsString(parent) && !cJSON_IsNull(parent))
            errorField = strBuild("parent");

        if (!cJSON_IsNumber(parent_dist) && !cJSON_IsNull(parent_dist))
            errorField = strBuild("parent_dist");

        if (!cJSON_IsNumber(solar_tilt))
            errorField = strBuild("solar_tilt");

        if (!cJSON_IsArray(color) || (cJSON_GetArraySize(color) != 3))
            errorField = strBuild("color");

        if (!cJSON_IsNumber(day_period))
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

        float orbitPeriodRaw = (cJSON_IsNull(orbit_period) ? 1.0f : (float)orbit_period->valuedouble);
        float parentDistanceRaw = (cJSON_IsNull(parent_dist) ? 0.0f : (float)parent_dist->valuedouble);

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
                    (float)radius->valuedouble,
                    orbitPeriodRaw,
                    parentRaw,
                    parentDistanceRaw,
                    (float)solar_tilt->valuedouble,
                    textureId,
                    has_texture,
                    (float)day_period->valuedouble
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