from sys import argv
from platform import system as pl_system
from PIL import Image

import os
import stat
import shutil
import subprocess


HELP_MESSAGE = """
< ---------------------------------------------- < Script Arguments > ---------------------------------------------- >

    * -build-depend: Compiles the dependencies to produce the resulting *.dll and *.lib files. 
                     Should only be used once when seting up the project but can be ommited later.
                     Can be used independently of -build-proj and -run.

    * -build-proj:   Links the dependencies' generated *.dll and *.lib files and compiles the project. 
                     The resulting executable is situated somewhere within ./build depending on the used C compiler 
                     (./build/Release for MSVC). It can be used independently of -build-depend and -run.

    * -run:          Paired with the /planets:* argument, it executes the generated executable found within the 
                     ./build directory. It can be used independently of -build-depend and -build-proj.

    * /planets:*:    Paired with the -run argument, it specifies the data of the astronomical objects found in 
                     ./data/*/.json. If unsure on what to load, use "/planets:the_solar_system" where the astronomical 
                     system's data that will be used are situated in ./data/the_solar_system.json. Read the 
                     documentation's "JSON data" subsection under the "Implementation" section for more details.

< ---------------------------------------------- <  User Controls  > --------------------------------------------- >

    * W, A, S, D (hold):    Standard camera movement controls, relative to its orientation (FWD, BCK, LFT, RGT).

    * Spacebar (hold):      Move camera upwards, along the y axis.

    * X (hold):             Move camera downwards, along the y axis.

    * ESC (toggle):         Open/Close the main menu.

    * P (toggle):           Open/Close the Planets' menu.

    * H (toggle):           Open/Close the HUD for diagnostic information.

For more details on user input and interaction, go to the "Interaction" section of the documentation.
"""


def onerror_handler(func, path: str, exc_info):
    """
    Error handler for `shutil.rmtree`.

    If the error is due to an access error (read only file)
    it attempts to add write permission and then retries.

    If the error is for another reason it re-raises the error.

    Usage : `shutil.rmtree(path, onerror=onerror)`
    """
    # Is the error an access error?
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise Exception(exc_info)


def build_library_msvc(sln_dir_abs: str, sln_name: str) -> bool:

    if os.path.exists("./build/Release/%s.dll" % (sln_name)):
        print("Warning: Dependency \"%s\" has already been built; `--build-depend` is ignored." % (sln_name))
        return True

    # Build FreeGLUT from source using the generated solution and MSVC
    command = ["cmd.exe", "/c", "vcvarsall.bat", "x64", "&&"]
    command += ["cd", sln_dir_abs, "&&"]
    command += ["msbuild", "%s.sln" % (sln_name), "/p:Configuration=Release", "/p:Platform=x64", "/t:%s" % (sln_name)]

    # MSVC's environment variables must be loaded before build process 
    msvc_dir = "C:\\Program Files\\Microsoft Visual Studio\\"
    msvc_ver = os.listdir(msvc_dir)
    msvc_ver.sort()
    msvc_dir += "%s\\Community\\VC\\Auxiliary\\Build" % (msvc_ver[-1])

    # Run vcvarsall.bat and capture the environment variables
    try:
        subprocess.run(
            command,
            cwd=R"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build", 
            check=True
        )
    except subprocess.CalledProcessError as e:
        print(e)
        return False

    return True


def build_library_make(sln_dir_abs: str, sln_name: str) -> bool:
    pass


def setup_dependency(
    dep_name: str, 
    dep_repo_url: str, 
    version_hex_checkout: str
) -> bool:

    try:
        # Download dependency from its remote repository.
        subprocess.run(
            args=["git", "clone", dep_repo_url], 
            cwd="./dependencies", 
            check=True
        )
        # Switch to specific repo version to avoid compatibility issues.
        subprocess.run(
            args=["git", "checkout", version_hex_checkout], 
            cwd="./dependencies/%s" % (dep_name), 
            check=True
        )

        # Create build directory for CMake's output files.
        os.mkdir("./dependencies/%s/build" % (dep_name))

        # Build dependency from source
        subprocess.run(
            args=["cmake", ".."], 
            cwd="./dependencies/%s/build" % (dep_name), 
            check=True
        )

    except subprocess.CalledProcessError as e:
        print(e)
        return False
    
    return True


if __name__ == '__main__':
    
    if "-help" in argv:
        print(HELP_MESSAGE)
        exit()
 
    if "-clear" in argv or "-cleanse" in argv:

        # Clears the project's cache
        if os.path.exists("./dependencies"):
            shutil.rmtree("./dependencies", onerror=onerror_handler)
        
        if os.path.exists("./.vscode"):
            shutil.rmtree("./.vscode", onerror=onerror_handler)

        if os.path.exists("./build"):
            shutil.rmtree("./build", onerror=onerror_handler)
            
        if os.path.exists("./data"):
            
            astro_system_dirs = [x for x in os.listdir("./data") if os.path.isdir("./data/%s" % (x))]
            
            for astro_dir in astro_system_dirs:
                
                bitmapTextureFiles = [x for x in os.listdir("./data/%s" % (astro_dir)) if x.endswith(".bmp")]
                
                for stellarObjectBitamp in bitmapTextureFiles:
                    os.remove("./data/%s/%s" % (astro_dir, stellarObjectBitamp))

        # Exit gracefully
        exit()


    def remove_suffix(input_string: str, suffix: str) -> str:
        if suffix and input_string.endswith(suffix):
            return input_string[:-len(suffix)]
        return input_string


    def remove_prefix(input_string: str, prefix: str) -> str:
        if prefix and input_string.startswith(prefix):
            return input_string[len(prefix):]
        return input_string

    
    astro_system_dirs = [x for x in os.listdir("./data") if os.path.isdir("./data/%s" % (x))]
    
    for astro_dir in astro_system_dirs:
        
        jpegTextureFiles = [x for x in os.listdir("./data/%s" % (astro_dir)) if x.endswith(".jpg")]
        
        for stellarObjectJpeg in jpegTextureFiles:

            obj_name = remove_suffix(stellarObjectJpeg, '.jpg')
            input_path = "./data/%s/%s.jpg" % (astro_dir, obj_name)
            output_path = "./data/%s/%s.bmp" % (astro_dir, obj_name)
            
            if os.path.exists(output_path):
                # Texture has already been converted from JPEG to BMP.
                continue
            
            try:
                print("Converting %s's texture from JPEG to Bitmap..." % (obj_name))
                
                with Image.open(input_path) as img:
                    img.save(output_path, format="BMP")
                    
            except Exception as e:
                print("An error occurred while converting %s to bitmap: %s" % (stellarObjectJpeg, str(e)))

    
    if not os.path.exists("./dependencies"):
        os.mkdir("./dependencies")
    
    if not os.path.exists("./dependencies/freeglut"):

        # Additional dependencies for Deviant-based Linux distros:
        #   sudo apt-get install -y libx11-dev libxi-dev libxkbcommon-dev libgl1-mesa-dev libegl1-mesa-dev libxrandr-dev libxext-dev

        # Installing CMake 3.23.5 for Linux:
        #   wget https://github.com/Kitware/CMake/releases/download/v3.23.5/cmake-3.23.5-linux-x86_64.tar.gz
        #   tar -xzvf cmake-3.23.5-linux-x86_64.tar.gz
        #   sudo mv cmake-3.23.5-linux-x86_64 /opt/cmake-3.23.5
        #   sudo ln -sf /opt/cmake-3.23.5/bin/* /usr/local/bin/

        # Verify the Installation
        #   cmake --version

        # Download FreeGLUT from its remote repository
        if not setup_dependency(
            "freeglut", 
            "https://github.com/freeglut/freeglut.git",
            "96c4b993aab2c1139d940aa6fc9d8955d4e019fa"
        ):
            print("Error when cloning FreeGLUT from source repository.")
            exit(1)

    if not os.path.exists("./dependencies/cJSON"):
        
        # Download cJSON from its remote repository
        if not setup_dependency(
            "cJSON", 
            "https://github.com/DaveGamble/cJSON.git",
            "12c4bf1986c288950a3d06da757109a6aa1ece38"
        ):
            print("Error when cloning FreeGLUT from source repository.")
            exit(1)


    system_platform = pl_system()

    print("Operating System family: %s" % (system_platform))

    if system_platform == "Windows":

        # MS Windows
        if "-build-depend" in argv:

            # Compile FreeGLUT library from source
            if not build_library_msvc(
                sln_dir_abs="%s\\dependencies\\freeglut\\build" % (os.getcwd()), 
                sln_name="freeglut"
            ):
                print("Compilation of the FreeGLUT library was unsuccessful.\n")
                exit(1)

            # Compile cJSON library from source
            if not build_library_msvc(
                sln_dir_abs="%s\\dependencies\\cJSON\\build" % (os.getcwd()), 
                sln_name="cjson"
            ):
                print("Compilation of the cJSON library was unsuccessful.\n")
                exit(1)

        if "-build-proj" in argv:

            # Compile Solar System Project from source
            if not os.path.exists("./build"):
                os.mkdir("./build")

            subprocess.run(
                args=["cmake", ".."], 
                cwd="./build", 
                check=True
            )

            if not build_library_msvc(
                sln_dir_abs="%s\\build" % (os.getcwd()), 
                sln_name="solar_system"
            ):
                exit(1)

        if "-run" in argv:
            
            # Load the program's user preferences.
            constants = "%s\\data\\constants.json" % (os.getcwd())

            astro_system_dir = [x for x in argv if x.startswith("/planets:")]
            astro_system_dir = remove_prefix(astro_system_dir[0], '/planets:')
            astro_system_dir = "%s\\data\\%s\\" % (os.getcwd(), astro_system_dir)

            subprocess.run(
                args=[".\\build\\Release\\solar_system.exe", constants, astro_system_dir], 
                check=True
            )

    elif system_platform == "Linux":
        
        # Linux Distro
        if "-build-depend" in argv:

            freeglut_bin_path = "./dependencies/freeglut/build/bin"

            if not os.path.exists(freeglut_bin_path) or not os.listdir(freeglut_bin_path):
                subprocess.run(
                    args=["make"],
                    cwd="./dependencies/freeglut/build",
                    check=True
                )

            subprocess.run(
                args=["make"],
                cwd="./dependencies/cJSON/build",
                check=True
            )

        if "-build-proj" in argv:

            if not os.path.exists("./build"):
                os.mkdir("./build")

            subprocess.run(
                args=["cmake", ".."],
                cwd="./build",
                check=True
            )

            subprocess.run(
                args=["make"],
                cwd="./build",
                check=True
            )

        if "-run" in argv:
            
            # Load the program's user preferences.
            constants = "%s/data/constants.json" % (os.getcwd())

            astro_system_dir = [x for x in argv if x.startswith("/planets:")]
            astro_system_dir = remove_prefix(astro_system_dir[0], '/planets:')
            astro_system_dir = "%s/data/%s/" % (os.getcwd(), astro_system_dir)

            subprocess.run(
                args=["./build/solar_system", constants, astro_system_dir], 
                check=True
            )

    elif system_platform == "Darwin":
        # MacOS
        pass

    elif system_platform == "Java":
        # ???
        pass

    else:
        raise Exception("Unrecognised System Platform: %s" % (system_platform))
