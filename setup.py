from sys import argv
from platform import system as pl_system
from PIL import Image


import os
import stat
import shutil
import subprocess
import numpy as np


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
                     ./data/*.json. If unsure on what to load, use "/planets:the_solar_system" where the astronomical 
                     system's data that will be used are situated in ./data/the_solar_system.json. Read the 
                     documentation's "JSON data" subsection under the "Implementation" section for more details.
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


def execute_binaries_msvc(sln_dir_abs: str, sln_name: str) -> bool:

    # Build FreeGLUT from source using the generated solution and MSVC
    command = "cmd.exe /c vcvarsall.bat x64 && "
    command += f"cd {sln_dir_abs} && "
    command += f"msbuild {sln_name}.sln /p:Configuration=Release /p:Platform=x64 /t:{sln_name}"

    # MSVC's environment variables must be loaded before build process 
    msvc_dir = f"C:\\Program Files\\Microsoft Visual Studio\\"
    msvc_ver = os.listdir(msvc_dir)
    msvc_ver.sort()
    msvc_dir += f"{msvc_ver[-1]}\\Community\\VC\\Auxiliary\\Build"

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


def setup_dependency(
    dep_name: str, 
    dep_repo_url: str, 
    version_hex_checkout: str
) -> bool:

    try:
        # Download dependency from its remote repository
        subprocess.run(f"git clone {dep_repo_url}", cwd="./dependencies", check=True)
        subprocess.run(f"git checkout {version_hex_checkout}", cwd=f"./dependencies/{dep_name}", check=True)

        # Create build directory for CMake's output files
        os.mkdir(f"./dependencies/{dep_name}/build")

        # Build dependency from source
        subprocess.run(f"cmake ..", cwd=f"./dependencies/{dep_name}/build", check=True)

    except subprocess.CalledProcessError as e:
        print(e)
        return False
    
    return True


if __name__ == '__main__':
    
    if "-help" in argv:
        print(HELP_MESSAGE)
        exit(0)

    if "-clear" in argv or "-cleanse" in argv:
        
        # Clears the project's cache
        if os.path.exists("./dependencies"):
            shutil.rmtree("./dependencies", onerror=onerror_handler)
        
        if os.path.exists("./.vscode"):
            shutil.rmtree("./.vscode", onerror=onerror_handler)

        if os.path.exists("./build"):
            shutil.rmtree("./build", onerror=onerror_handler)
            
        if os.path.exists("./data"):
            
            astro_system_dirs = [x for x in os.listdir("./data") if os.path.isdir(f"./data/{x}")]
            
            for astro_dir in astro_system_dirs:
                
                bitmapTextureFiles = [x for x in os.listdir(f"./data/{astro_dir}") if x.endswith(".bmp")]
                
                for stellarObjectBitamp in bitmapTextureFiles:
                    os.remove(f"./data/{astro_dir}/{stellarObjectBitamp}")

        # Exit gracefully
        exit()

    
    astro_system_dirs = [x for x in os.listdir("./data") if os.path.isdir(f"./data/{x}")]
    
    for astro_dir in astro_system_dirs:
        
        jpegTextureFiles = [x for x in os.listdir(f"./data/{astro_dir}") if x.endswith(".jpg")]
        
        for stellarObjectJpeg in jpegTextureFiles:
            
            obj_name = stellarObjectJpeg.removesuffix('.jpg')
            input_path = f"./data/{astro_dir}/{obj_name}.jpg"
            output_path = f"./data/{astro_dir}/{obj_name}.bmp"
            
            if os.path.exists(output_path):
                # Texture has already been converted from JPEG to BMP.
                continue
            
            try:
                print(f"Converting {obj_name}'s texture from JPEG to Bitmap...")
                
                with Image.open(input_path) as img:
                    img.save(output_path, format="BMP")
                    
            except Exception as e:
                print(f"An error occurred while converting {stellarObjectJpeg} to bitmap: {e}")

    
    if not os.path.exists("./dependencies"):
        os.mkdir("./dependencies")
    
    if not os.path.exists("./dependencies/freeglut"):
        
        # Download FreeGLUT from its remote repository
        if not setup_dependency(
            "freeglut", 
            "https://github.com/freeglut/freeglut.git",
            "96c4b993aab2c1139d940aa6fc9d8955d4e019fa"
        ):
            print("Error when cloning FreeGLUT from source repository.")
            exit(1)

    if not os.path.exists("./dependencies/cjson"):
        
        # Download cJSON from its remote repository
        if not setup_dependency(
            "cjson", 
            "https://github.com/DaveGamble/cJSON.git",
            "12c4bf1986c288950a3d06da757109a6aa1ece38"
        ):
            print("Error when cloning FreeGLUT from source repository.")
            exit(1)


    system_platform = pl_system()

    print(f"Operating System family: {system_platform}")

    if system_platform == "Windows":

        # MS Windows
        if "-build-depend" in argv:

            # Compile FreeGLUT library from source
            if not execute_binaries_msvc(
                sln_dir_abs=f"{os.getcwd()}\\dependencies\\freeglut\\build", 
                sln_name="freeglut"
            ):
                print("Compilation of the FreeGLUT library was unsuccessful.\n")
                exit(1)

            # Compile cJSON library from source
            if not execute_binaries_msvc(
                sln_dir_abs=f"{os.getcwd()}\\dependencies\\cjson\\build", 
                sln_name="cjson"
            ):
                print("Compilation of the cJSON library was unsuccessful.\n")
                exit(1)

        if "-build-proj" in argv:

            # Compile Solar System Project from source
            if not os.path.exists("./build"):
                os.mkdir("./build")

            subprocess.run("cmake ..", cwd="./build", check=True)

            if not execute_binaries_msvc(
                sln_dir_abs=f"{os.getcwd()}\\build", 
                sln_name="solar_system"
            ):
                exit(1)

        if "-run" in argv:
            
            # Load the program's user preferences.
            constants = f"{os.getcwd()}\\data\\constants.json"

            astro_system_dir = [x for x in argv if x.startswith("/planets:")]
            astro_system_dir = astro_system_dir[0].removeprefix('/planets:')
            astro_system_dir = f"{os.getcwd()}\\data\\{astro_system_dir}\\"

            subprocess.run(
                f".\\build\\Release\\solar_system.exe {constants} {astro_system_dir}", 
                check=True
            )

    elif system_platform == "Linux":
        # Linux Distro
        pass
    elif system_platform == "Darwin":
        # MacOS
        pass
    elif system_platform == "Java":
        # ???
        pass
    else:
        raise Exception(f"Unrecognised System Platform: {system_platform}")
