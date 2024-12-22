# OpenGL implementation of the Solar System

## Preface
3D simulation of the Copernican astronomical model. The radii and distances of astronomical objects are to scale. Implemented using Python3, C, FreeGLUT and cJSON.

<p align="center">
  <img src="./media/exhibition.gif" alt="Solar System Exhibition GIF">
  <br>
  <i> Preview of the rendered scene as the camera slowly moves away. </i>
</p>

<br>

---

## Contents

1. [System Requirements](#system-requirements)

2. [Setup](#setup)

3. [Implementation](#implementation)

    1. [JSON data](#i-json-data)

    2. [Dependencies](#ii-dependencies)

        1. [cJSON](#cjson)

        2. [FreeGLUT](#freeglut)

    3. [Build Automation](#iii-build-automation)

    4. [Interaction](#iv-interaction)

    5. [Classes](#v-classes)

        1. [CustomTypes](#customtypes)

        2. [StellarObject](#stellarobject)

        3. [Camera](#camera)

        4. [AmbientStars](#ambientstars)

        5. [MenuScreen](#menuscreen)

        6. [TextRendering](#textrendering)

        7. [HeadsUpDisplay](#headsupdisplay)

        8. [Timer](#timer)


<br>

---

## System Requirements

This project demands the existence of a base **Python3** interpeter, **CMake ver >3.1** and a C compiler. In the future the project will be supported for different C compilers and operating systems (Windows, Linux, MacOS). However, for the time being support is available only on machines with **MS Windows OS** and the **MSVC compiler**.

<br>

---


## Setup 

1. Clone the repository:
    ```
    git clone https://github.com/DimYfantidis/my_solar_system.git
    ```

2. Within the repo's directory, paste the command:
    ``` 
    python setup.py -build-depend -build-proj -run /planets:the_solar_system
    ```
    The `setup.py` script automatically creates and handle's the project's file structure, downloads and compiles [dependencies](#ii-dependencies), links them to the project, compiles the project and executes the generated executable with the `/planets:*` command line argument.

    For more information on the build automation python script, refer to the [Build Automation](#iii-build-automation) subsection or type (within the repo's dir):
    ```
    python setup.py -help
    ```

<br>

---


## Implementation

### I. JSON data

All the simulation's data are loaded from external JSON files, found within the `./data` directory. Two JSON files are loaded and parsed, i.e. `_constants.json` and any other JSON file with astronomical data.

The user is encouraged to manipulate `_constants.json`'s data to fit their preferences. Currently, it is is expected to have the following structure:

```json
{
    "window_dimensions" : {
        "width" : 2560,
        "height" : 1440
    },

    "fullscreen" : true,

    "framerate" : 144.0
}
```

The second JSON file that contains the astronomical system's data (e.g. `the_solar_system.json`) is expected to comprise of a single array of objects under the **"Astronomical Objects"** key. The array's elements specify each astronomical object found within the system, as well as its parameters which are `name`, `radius` (AU), `lin_velocity` (AU/h), `parent`, `parent_dist` (AU), `solar_tilt` (degrees) and `color`. For more information on those fields, refer to the [StellarObject class](#stellarobject).


**Note:** The simulation data should not be confused with user input data. While "simulation data" are also inpute data, the term "user input" refers to keyboard and mouse input for interacting with the simulation.

<br>

### II. Dependencies

#### cJSON

[cJSON](https://github.com/DaveGamble/cJSON) is an open-source ultra-lightweight JSON parser in ANSI C. It is used for parsing the JSON files found under the `./data` directory.

#### FreeGLUT

[FreeGLUT](https://github.com/freeglut/freeglut) is a free and open-source implementation of the OpenGL Utility Toolkit (GLUT). It is used for managing windows, rendering the 3D world and interacting with it. For more information on interaction, i.e. user input, read [Interaction](#iv-interaction).

<br>

### III. Build Automation

*TODO: write documentation*

<br>


### IV. Interaction

*TODO: write documentation*

<br>

### V. Classes

This subsection provides an in-depth explanation of the program's modules of implementation that are found within the `./include` directory.

<a id="customtypes"></a>

* `CustomTypes.h`:


<a id="stellarobject"></a>

* `StellarObject.h`:


<a id="camera"></a>

* `Camera.h`:


<a id="ambientstars"></a>

* `AmbientStars.h`:


<a id="menuscreen"></a>

* `MenuScreen.h`:


<a id="textrendering"></a>

* `TextRendering`:


<a id="headsupdisplay"></a>

* `HeadsUpDisplay.h`:


<a id="timer"></a>

* `Timer.h`:
