#ifndef STELLAR_OBJECT_H
#define STELLAR_OBJECT_H

#include <cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#include "CustomTypes.h"
#include "TextRendering.h"
#include "KeyboardCallback.h"


struct StellarObject
{
    char* name;
    GLUquadric* quad;
    struct StellarObject* parent;
    vector3f position;
    float radius;
    float parametric_angle;
    float velocity;
    float parent_dist;
    float solar_tilt;
    vector3ub color;
};
typedef struct StellarObject StellarObject;


StellarObject* initStellarObject(
    const char* name, 
    float radius, 
    float lin_velocity, 
    StellarObject* parent, 
    float parent_dist,
    float solar_tilt
) 
{
#ifdef PROJ_DEBUG
    printf("Creating StellarObject of radius %.2f\n", radius);
#endif
    StellarObject* p = (StellarObject*)malloc(sizeof(StellarObject));

    p->name = strBuild(name);
    p->radius = AUtoR(radius);
    p->parent = parent;
    p->solar_tilt = solar_tilt;
    p->quad = gluNewQuadric();
    p->parametric_angle = (float)(-M_PI);
    p->parent_dist = AUtoR(parent_dist);
    p->velocity = AUtoR(lin_velocity);

    if (parent_dist != .0f) {
        p->velocity /= parent_dist;
    }

    memset(p->position, (int).0f, sizeof(p->position));
    memset(p->position, (int)0x00, sizeof(p->position));

    gluQuadricDrawStyle(p->quad, GLU_FILL);

    return p;
}

StellarObject* colorise3f(StellarObject* p, float red, float green, float blue)
{
    p->color[0] = (ubyte)(red * 255);
    p->color[1] = (ubyte)(green * 255);
    p->color[2] = (ubyte)(blue * 255);
    
    return p;
}

StellarObject* colorise3ub(StellarObject* p, ubyte red, ubyte green, ubyte blue)
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
        free(p);
    }
}

void updateStellarObject(StellarObject* p, float speed_factor)
{
    p->parametric_angle += speed_factor * p->velocity;

    if (p->parametric_angle > M_PI) {
        p->parametric_angle -= (float)(2 * M_PI);
    }

    float sin_ang = sinf(p->parametric_angle);
    float cos_ang = cosf(p->parametric_angle);

    p->position[0] = cos_ang * p->parent_dist;
    p->position[2] = sin_ang * p->parent_dist;
}

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

void renderStellarObject(StellarObject* p, bool render_trajectory, StellarObject** ancestors, int* n_ancestors)
{
    bool uses_cached_ancestors = (ancestors == NULL ? false : true);

    int tmp_n_ancestors;


    glColor3ubv(p->color);
    glPushMatrix();

    if (n_ancestors == NULL)
        n_ancestors = &tmp_n_ancestors;

    if (!uses_cached_ancestors)
        ancestors = getStellarObjectAncestors(p, n_ancestors);

    // Apply the transformations of all previous astrological systems' anchors. 
    for (int i = *n_ancestors - 1; i >= 0; --i) 
    {
        glRotatef(ancestors[i]->solar_tilt, 0, 0, 1);

        glTranslatef(
            ancestors[i]->position[0],
            ancestors[i]->position[1],
            ancestors[i]->position[2]
        );
    }

    if (!uses_cached_ancestors)
        free(ancestors);

    glRotatef(p->solar_tilt, 0, 0, 1);

    // The above transformations are saved as they will be 
    // used for rendering the StellarObject's trajectory as well.
    glPushMatrix();

    glTranslatef(
        p->position[0], 
        p->position[1], 
        p->position[2]
    );

    // Render planet
    gluSphere(p->quad, p->radius, 64, 32);

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

        glScalef(p->parent_dist, p->parent_dist, p->parent_dist);

        glBegin(GL_LINE_LOOP);
        for (float theta = (float)(-M_PI); theta < (float)M_PI; theta += (float)M_PI / 100.0f) 
        {
            glVertex3f(cosf(theta), .0f, sinf(theta));
        }
        glEnd();
    }
    glPopMatrix();
}

StellarObject** loadAllStellarObjects(int* array_size, const char* json_filename)
{
    StellarObject** dest_array;

    *array_size = 0;

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

    dest_array = (StellarObject **)malloc(cJSON_GetArraySize(allStellarObjects) * sizeof(StellarObject *));

    cJSON_ArrayForEach(iterator, allStellarObjects)
    {
        if (!cJSON_IsObject(iterator))
        {
            fprintf(
                stderr,
                "Error: %s should only contain JSON objects; Inspect arrray item idx.#%d in \"%s\"", 
                allStellarObjects->string, *array_size, json_filename
            );

            for (int i = 0; i < *array_size; ++i)
                deleteStellarObject(dest_array[i]);

            free(dest_array);
            
            return NULL;
        }

        static const char* err_message = "Error: JSON array idx.#%d - StellarObject's `%s` field is invalid; Inspect \"%s\".\n";
        char* err_field = NULL;


        cJSON *name = cJSON_GetObjectItemCaseSensitive(iterator, "name");
        cJSON *radius = cJSON_GetObjectItemCaseSensitive(iterator, "radius");
        cJSON *lin_velocity = cJSON_GetObjectItemCaseSensitive(iterator, "lin_velocity");
        cJSON *parent = cJSON_GetObjectItemCaseSensitive(iterator, "parent");
        cJSON *parent_dist = cJSON_GetObjectItemCaseSensitive(iterator, "parent_dist");
        cJSON *solar_tilt = cJSON_GetObjectItemCaseSensitive(iterator, "solar_tilt");
        cJSON *color = cJSON_GetObjectItemCaseSensitive(iterator, "color");


        if (!cJSON_IsString(name) || name->valuestring == NULL)
            err_field = name->string;

        if (!cJSON_IsNumber(radius))
            err_field = radius->string;

        if (!cJSON_IsNumber(lin_velocity))
            err_field = lin_velocity->string;

        if (!cJSON_IsString(parent) && !cJSON_IsNull(parent))
            err_field = parent->string;

        if (!cJSON_IsNumber(parent_dist))
            err_field = parent_dist->string;

        if (!cJSON_IsNumber(solar_tilt))
            err_field = solar_tilt->string;

        if (!cJSON_IsArray(color) || (cJSON_GetArraySize(color) != 3))
            err_field = color->string;


        if (err_field != NULL)
        {
            fprintf(stderr, err_message, *array_size, err_field, json_filename);
            free(dest_array);
            return NULL;
        }

        StellarObject* parent_raw = NULL;

        if (!cJSON_IsNull(parent))
        {
            int i;

            for (i = 0; i < *array_size; ++i)
            {
                if (strcmp(parent->valuestring, dest_array[i]->name) != 0)
                    continue;
                
                parent_raw = dest_array[i];
                break;
            }

            if (i == *array_size)
            {
                printf(
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
        
        cJSON* r = cJSON_GetArrayItem(color, 0);
        cJSON* g = cJSON_GetArrayItem(color, 1);
        cJSON* b = cJSON_GetArrayItem(color, 2);

        dest_array[*array_size] = colorise3ub(
                initStellarObject(
                    name->valuestring,
                    (float)radius->valuedouble,
                    (float)lin_velocity->valuedouble,
                    parent_raw,
                    (float)parent_dist->valuedouble,
                    (float)solar_tilt->valuedouble
            ),
            (ubyte)r->valueint, (ubyte)g->valueint, (ubyte)b->valueint
        );

        *array_size += 1;
    }

    cJSON_Delete(json);
    free(buffer);

    dest_array = (StellarObject **)realloc(dest_array, (*array_size) * sizeof(StellarObject *));

    return dest_array;
}

#endif // STELLAR_OBJECT_H