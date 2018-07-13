cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

project(gl-compute)

#set(MFEM_PATH ${CMAKE_SOURCE_DIR}/../mfem)

add_definitions(-std=c++11 -ggdb)
include_directories(${CMAKE_BINARY_DIR})
include(cmake/file2cpp.cmake)

if(EXISTS /usr/bin/qmake-qt4)
  # disable Qt5 (default on Ubuntu 13.04)
  set(QT_QMAKE_EXECUTABLE /usr/bin/qmake-qt4)
endif()

# use Qt4 support
find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

# OpenGL
add_definitions(-DGL_GLEXT_PROTOTYPES=1)
find_package(OpenGL 4.3 REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# OpenGL Math (GLM)
find_package(GLM REQUIRED)

#include_directories(${MFEM_PATH})


#### mandelbrot ####

set(mandelbrot_SOURCES
    mandelbrot.cpp mandelbrot.hpp shader.hpp
)
set(mandelbrot_MOC_HEADERS
    mandelbrot.hpp
)
qt4_wrap_cpp(mandelbrot_MOC_FILES ${mandelbrot_MOC_HEADERS})

file_to_cpp(mandelbrot_DATA shaders::quad shaders/quad.glsl)
file_to_cpp(mandelbrot_DATA shaders::palette shaders/palette.glsl)
file_to_cpp(mandelbrot_DATA shaders::mandelbrot shaders/mandelbrot.glsl)

add_executable(mandelbrot
    ${mandelbrot_SOURCES} ${mandelbrot_MOC_FILES} ${mandelbrot_DATA}
)
target_link_libraries(mandelbrot
    ${QT_LIBRARIES} ${OPENGL_LIBRARIES}
)


#### geometry ####

set(geometry_SOURCES
    geometry.cpp geometry.hpp shader.hpp
)
set(geometry_MOC_HEADERS
    geometry.hpp
)
qt4_wrap_cpp(geometry_MOC_FILES ${geometry_MOC_HEADERS})

file_to_cpp(geometry_DATA shaders::isosurface shaders/isosurface.glsl)
file_to_cpp(geometry_DATA shaders::palette shaders/palette.glsl)
file_to_cpp(geometry_DATA shaders::mesh shaders/mesh.glsl)

add_executable(geometry
    ${geometry_SOURCES} ${geometry_MOC_FILES} ${geometry_DATA}
)
target_link_libraries(geometry
    ${QT_LIBRARIES} ${OPENGL_LIBRARIES}
)
