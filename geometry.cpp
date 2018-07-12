#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vector>

#include <QApplication>
#include <QString>

/*#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>*/

#include "geometry.hpp"

const double PanSpeed = 0.005;
const double RotateSpeed = 0.4;


RenderWidget::RenderWidget(const QGLFormat &format)

  : QGLWidget(format, (QWidget*) 0)

  , curSize(-1, -1)
  , texSize(-1, -1)

  , scale(0.)
  , panX(0.), panY(0.)
{
   grabKeyboard();
}

RenderWidget::~RenderWidget()
{
}


#include "shaders/isosurface.glsl.hpp"
#include "shaders/palette.glsl.hpp"
#include "shaders/mesh.glsl.hpp"

void RenderWidget::compileShaders()
{
   const int version = 430;

   progMesh.link(
       VertexShader(version, {shaders::palette, shaders::mesh}),
       FragmentShader(version, {shaders::palette, shaders::mesh}));

   progCompute.link(
       ComputeShader(version, {shaders::isosurface}));
}

void RenderWidget::initializeGL()
{
   std::cout << "OpenGL version: " << glGetString(GL_VERSION)
             << ", renderer: " << glGetString(GL_RENDERER) << std::endl;

   GLint major, minor;
   glGetIntegerv(GL_MAJOR_VERSION, &major);
   glGetIntegerv(GL_MINOR_VERSION, &minor);

   if (major*10 + minor < 43) {
      throw std::runtime_error(
         "OpenGL version 4.3 or higher is required to run this program.");
   }

   compileShaders();

   // create a shader buffer to store generated triangles
   glGenBuffers(1, &ssbo);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
   glBufferData(GL_SHADER_STORAGE_BUFFER, 1000/*FIXME*/*sizeof(float), NULL, GL_STATIC_DRAW);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

   // create a buffer for an atomic uint (the number of generated triangles)
   glGenBuffers(1, &atomic);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, atomic);
   glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), NULL, GL_STATIC_COPY/*FIXME?*/);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, atomic);

   // create a VAO for drawing, bind the SSBO holding vertices
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, ssbo);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
}

void RenderWidget::resizeGL(int width, int height)
{
   glViewport(0, 0, width, height);
   curSize = QSize(width, height);
   aspect = (double) width / height;
}

void RenderWidget::paintGL()
{
   glClearColor(1, 1, 1, 1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPolygonMode(GL_FRONT_AND_BACK, /*wireframe*/0 ? GL_LINE : GL_FILL);

   // reset the atomic counter
   GLuint num_triangles = 0;
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, atomic);
   glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &num_triangles);

   // launch the compute shader
   progCompute.use();
   glDispatchCompute(20, 1, 1);
   glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   // read the number of triangles generated
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, atomic);
   glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &num_triangles);

   // draw vertices generated into the buffer
   progMesh.use();
   glBindVertexArray(vao);
   glDrawArrays(GL_TRIANGLES, 0, 3*num_triangles);
}


void RenderWidget::mousePressEvent(QMouseEvent *event)
{
   lastPos = event->pos();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
   double deltaX(event->x() - lastPos.x());
   double deltaY(event->y() - lastPos.y());

   bool leftButton();
   bool rightButton(event->buttons() & Qt::RightButton);

   if (event->buttons() & Qt::RightButton)
   {
   }
   else if (event->buttons() & Qt::LeftButton)
   {
      panX += deltaX;
      panY -= deltaY;
   }

   lastPos = event->pos();
   updateGL();
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
   //zoom(-double(event->delta()) / 10);
   updateGL();
}

void RenderWidget::keyPressEvent(QKeyEvent * event)
{
   switch (event->key())
   {
   case Qt::Key_Q:
      parentWidget()->close();
      break;

   case Qt::Key_Left:
      break;

   case Qt::Key_Right:
      break;

   case Qt::Key_Minus:
      break;

   case Qt::Key_Plus:
      break;
   }
   updateGL();
}


MainWindow::MainWindow(QWidget* gl)
{
    setCentralWidget(gl);
    setWindowTitle("geometry");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGLFormat glf = QGLFormat::defaultFormat();
    /*glf.setSampleBuffers(true);
    glf.setSamples(8);*/

    RenderWidget* gl =
         new RenderWidget(glf);

    MainWindow wnd(gl);
    gl->setParent(&wnd);

    QSize size(1200, 900);
    wnd.resize(size);
    wnd.show();

    return app.exec();
}
