#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vector>

#include <QApplication>
#include <QString>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "isosurface.hpp"

#include "tables.hpp"


RenderWidget::RenderWidget(const QGLFormat &format)

  : QGLWidget(format, (QWidget*) 0)

  , curSize(-1, -1)
  , texSize(-1, -1)

  , scale(0.5)
  , rotateX(0.), rotateY(0.)
  , isoValue(0.5)
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

/*   if (major*10 + minor < 43) {
      throw std::runtime_error(
         "OpenGL version 4.3 or higher is required to run this program.");
   }*/

   compileShaders();

   // adjust the tables
   for (int i = 0, j; i < 256; i++)
   {
      for (j = 0; mcTables.triTable[i][j] != -1; j++) {}
      // store vertex count
      mcTables.triTable[i][15] = j;
   }

   // create a shader buffer to store generated triangles (triples of vertices)
   glGenBuffers(1, &ssboVert);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboVert);
   glBufferData(GL_SHADER_STORAGE_BUFFER, 12*1024*1024/*FIXME*/*sizeof(float), NULL, GL_STATIC_DRAW);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboVert);

   // buffer for an atomic uint (the number of generated vertices)
   glGenBuffers(1, &ssboCount);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboCount);
   glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboCount);

   // buffer for marching cubes tables
   glGenBuffers(1, &ssboTables);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTables);
   glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(mcTables), &mcTables, GL_STATIC_DRAW);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboTables);

   // create a VAO for drawing, bind the SSBO holding vertices
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, ssboVert);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

   updateIsosurface();
}

void RenderWidget::updateIsosurface()
{
   // reset the atomic counter
   vertCount = 0;
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboCount);
   glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &vertCount);

   progCompute.use();
   const int num_voxels = 20;
   glUniform1f(progCompute.uniform("voxel_size"), 2.0 / num_voxels);
   glUniform1f(progCompute.uniform("iso_value"), isoValue);

   // launch the compute shader and wait for completion
   glDispatchCompute(num_voxels, num_voxels, num_voxels);
   glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   // read the number of vertices generated
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboCount);
   glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &vertCount);
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
   glPolygonMode(GL_FRONT_AND_BACK, /*wireframe*/1 ? GL_LINE : GL_FILL);

   // setup the model-view-projection transform
   glm::mat4 proj = glm::perspective(glm::radians(45.0), aspect, 0.001, 100.0);
   glm::mat4 view(1.0);
   view = glm::translate(view, glm::vec3(0, 0, -1));
   view = glm::scale(view, glm::vec3(scale, scale, scale));
   view = glm::rotate(view, glm::radians(rotateX), glm::vec3(1, 0, 0));
   view = glm::rotate(view, glm::radians(rotateY), glm::vec3(0, 1, 0));
   glm::mat4 MVP = proj * view;

   // draw vertices generated into the ssboVert
   progMesh.use();
   glUniformMatrix4fv(progMesh.uniform("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
   glBindVertexArray(vao);
   glDrawArrays(GL_TRIANGLES, 0, vertCount);
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
      scale -= 0.002 * deltaY;
      scale = std::max(scale, 0.0001f);
   }
   else if (event->buttons() & Qt::LeftButton)
   {
      rotateX += deltaY;
      rotateY += deltaX;
   }

   lastPos = event->pos();
   updateGL();
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
   isoValue += (double(event->delta()) / 120) / 50;
   updateIsosurface();
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
    setWindowTitle("isosurface");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGLFormat glf = QGLFormat::defaultFormat();
    glf.setSampleBuffers(true);
    glf.setSamples(8);

    RenderWidget* gl =
         new RenderWidget(glf);

    MainWindow wnd(gl);
    gl->setParent(&wnd);

    QSize size(1200, 900);
    wnd.resize(size);
    wnd.show();

    return app.exec();
}
