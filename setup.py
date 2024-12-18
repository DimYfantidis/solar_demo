from sys import argv
from platform import system as pl_system

import os
import stat
import shutil
import subprocess


def onerror_handler(func, path: str, exc_info):
    """
    Error handler for ``shutil.rmtree``.

    If the error is due to an access error (read only file)
    it attempts to add write permission and then retries.

    If the error is for another reason it re-raises the error.
    
    Usage : ``shutil.rmtree(path, onerror=onerror)``
    """
    # Is the error an access error?
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise Exception(exc_info)


def compile_binaries(sln_dir_abs: str, sln_name: str) -> bool:

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


if __name__ == '__main__':
    
    if "-clear" in argv or "-cleanse" in argv:
        
        # Clears the project's cache
        if os.path.exists("./dependencies"):
            shutil.rmtree("./dependencies", onerror=onerror_handler)
        
        if os.path.exists("./.vscode"):
            shutil.rmtree("./.vscode", onerror=onerror_handler)
            
        if os.path.exists("./build"):
            shutil.rmtree("./build", onerror=onerror_handler)

        # Exit gracefully
        exit()
    
    if not os.path.exists("./dependencies"):
        os.mkdir("./dependencies")
    
    if not os.path.exists("./dependencies/freeglut"):

        # Download freeglut from its remote repository
        subprocess.run("git clone --branch v3.6.0 https://github.com/freeglut/freeglut.git", cwd="./dependencies", check=True)
        subprocess.run("git checkout 96c4b993aab2c1139d940aa6fc9d8955d4e019fa", cwd="./dependencies/freeglut", check=True)

        # Create build directory for CMake's output files
        os.mkdir("./dependencies/freeglut/build")

        # Build FreeGLUT from source
        subprocess.run("cmake ..", cwd="./dependencies/freeglut/build", check=True)

    system_platform = pl_system()

    print(f"Operating System family: {system_platform}")

    if system_platform == "Windows":
        
        # MS Windows
        if "-build-freeglut" in argv:
            
            # Compile FreeGLUT library from source
            freeglut_success = compile_binaries(
                sln_dir_abs=f"{os.getcwd()}\\dependencies\\freeglut\\build", 
                sln_name="freeglut"
            )
            
            if not freeglut_success:
                print("Compilation of the FreeGLUT library was unsuccessful.\n")
                exit(1)
        
        if "-build-proj" in argv:
            
            # Compile Solar System Project from source
            if not os.path.exists("./build"):
                os.mkdir("./build")

            subprocess.run("cmake ..", cwd="./build", check=True)
            
            proj_success = compile_binaries(
                sln_dir_abs=f"{os.getcwd()}\\build", 
                sln_name="solar_system"
            )
            
            if not proj_success:
                exit(1)
            
        if "-run" in argv:
            subprocess.run(R".\build\Release\solar_system.exe", check=True)

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