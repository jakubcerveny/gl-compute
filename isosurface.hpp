#ifndef gl_compute_isosurface_hpp_included__
#define gl_compute_isosurface_hpp_included__

#include <GL/gl.h>
#include <GL/glu.h>

#include <QtGui>
#include <QGLWidget>
#include <QSize>

#include "shader.hpp"


class RenderWidget : public QGLWidget
{
   Q_OBJECT

public:
   RenderWidget(const QGLFormat &format);

   virtual ~RenderWidget();

protected:

   Program progMesh, progCompute;
   GLuint vao, ssboVert, ssboCount, ssboTables;

   void compileShaders();

   virtual void initializeGL();
   virtual void resizeGL(int width, int height);
   virtual void paintGL();

   virtual void mousePressEvent(QMouseEvent *event);
   virtual void mouseMoveEvent(QMouseEvent *event);
   virtual void wheelEvent(QWheelEvent *event);
   virtual void keyPressEvent(QKeyEvent * event);

   QSize curSize, texSize;
   double aspect;
   QPoint lastPos;

   float scale;
   float rotateX, rotateY;
   float isoValue;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* gl);
};


#endif // gl_compute_isosurface_hpp_included__
