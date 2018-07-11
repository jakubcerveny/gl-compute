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
#find_package(GLM REQUIRED)

#find_package(Eigen3 3.2 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})

#include_directories(${MFEM_PATH})

set(gl-compute_SOURCES
    render.cpp render.hpp
    main.cpp main.hpp
    shader.hpp
)

set(gl-compute_MOC_HEADERS
    main.hpp
    render.hpp
)

qt4_wrap_cpp(gl-compute_MOC_FILES ${gl-compute_MOC_HEADERS})

file_to_cpp(gl-compute_DATA shaders::quad shaders/quad.glsl)
file_to_cpp(gl-compute_DATA shaders::compute shaders/compute.glsl)
file_to_cpp(gl-compute_DATA shaders::palette shaders/palette.glsl)

add_executable(gl-compute
    ${gl-compute_SOURCES}
    ${gl-compute_MOC_FILES}
    ${gl-compute_DATA}
)

target_link_libraries(gl-compute
    ${QT_LIBRARIES}
    ${OPENGL_LIBRARIES}
#    ${MFEM_PATH}/libmfem.a
)
