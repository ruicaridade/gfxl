cmake_minimum_required(VERSION 3.0)

project(gfxl)

# Define compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)

# Platform
set(PLATFORM Win32)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -fdiagnostics-color")

# Linker flags [remove console: -mwindows]
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Project definitions
set(DEFINITIONS -DGFXL_GLM -DGFXL_OPENGL -DWIN32_SANDBOX)

# Define the target link libraries
set(LIBRARIES SDL2 SDL2main)

# Set the output to the bin folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM})

# Include directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/3rdparty)

# Link directories
set(LINK_DIR ${CMAKE_SOURCE_DIR}/lib/${PLATFORM})

# Source directory
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# ================================================

# Set up directories
include_directories(${INCLUDE_DIR})
link_directories(${LINK_DIR})

# Recursively include every *.cpp file inside src folder
file(GLOB_RECURSE SOURCES_CPP ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE SOURCES_C ${SOURCE_DIR}/*.c)
add_executable(${PROJECT_NAME} ${SOURCES_CPP} ${SOURCES_C})

# Add the previously set definitions before linking
add_definitions(${DEFINITIONS})

# Link the libraries to the executable
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})