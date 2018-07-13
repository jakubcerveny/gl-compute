# gl-compute

Simple tests of OpenGL 4.3 compute shaders.

![gl-compute](https://raw.githubusercontent.com/jakubcerveny/gl-compute/master/data/screen.png)

Note that you need a recent OS to run this. Ubuntu 16.04 or newer should do.

### Compiling

We need CMake, Qt4 and GLM:
```
$ sudo apt install cmake libqt4-dev libglm-dev
```

Configure, compile and run:
```
$ mkdir build; cd build
$ cmake ..
$ make
$ ./mandelbrot
$ ./isosurface
```

### Troubleshooting

Check what OpenGL versions your Linux system supports:
```
$ glxinfo | grep version
server glx version string: 1.4
client glx version string: 1.4
GLX version: 1.4
    Max core profile version: 4.5
    Max compat profile version: 3.0
    Max GLES1 profile version: 1.1
    Max GLES[23] profile version: 3.2
OpenGL core profile version string: 4.5 (Core Profile) Mesa 17.2.8
OpenGL core profile shading language version string: 4.50
OpenGL version string: 3.0 Mesa 17.2.8
OpenGL shading language version string: 1.30
OpenGL ES profile version string: OpenGL ES 3.2 Mesa 17.2.8
OpenGL ES profile shading language version string: OpenGL ES GLSL ES 3.20
```

On some systems, OpenGL 4.3 may not be enabled by default. Try running
with the following environment variable:
```
$ MESA_GL_VERSION_OVERRIDE=4.3 ./mandelbrot
```
