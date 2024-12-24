cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(solar_system LANGUAGES C)

include_directories(
    ./include
    ./dependencies/freeglut/include
    ./dependencies/cJSON
)

# Specify the library directories
link_directories(./dependencies/freeglut/build/lib/Release)
link_directories(./dependencies/cJSON/build/Release)

# Add source files
file(GLOB SOURCES "src/main.c")

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} freeglut)
target_link_libraries(${PROJECT_NAME} cjson)

# Post-build step to copy the DLL
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/dependencies/freeglut/build/bin/Release/freeglut.dll"
    "${CMAKE_SOURCE_DIR}/dependencies/cJSON/build/Release/cjson.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
