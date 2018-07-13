#ifndef gl_compute_mandelbrot_hpp_included__
#define gl_compute_mandelbrot_hpp_included__

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

   virtual ~RenderWidget() {}

protected:

   Program progQuad, progCompute;
   GLuint vao, tex;

   void compileShaders();
   void createTexture(QSize size);

   virtual void initializeGL();
   virtual void resizeGL(int width, int height);
   virtual void paintGL();

   virtual void mousePressEvent(QMouseEvent *event);
   virtual void mouseMoveEvent(QMouseEvent *event);
   virtual void wheelEvent(QWheelEvent *event);
   virtual void keyPressEvent(QKeyEvent * event);

   void zoom(double delta);

   QSize curSize, texSize;
   double aspect;
   QPoint lastPos, centerPos;

   double scale;
   double panX, panY;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* gl);
};


#endif // gl_compute_mandelbrot_hpp_included__
