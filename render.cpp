#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vector>

#include <QString>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render.hpp"
//#include "palette.hpp"

#include "shaders/quad.glsl.hpp"
#include "shaders/compute.glsl.hpp"

const double PanSpeed = 0.005;
const double RotateSpeed = 0.4;


RenderWidget::RenderWidget(const QGLFormat &format)

  : QGLWidget(format, (QWidget*) 0)

  , tex(0)
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

void RenderWidget::compileShaders()
{
   const int version = 430;

   progQuad.link(
       VertexShader(version, {shaders::quad}),
       FragmentShader(version, {shaders::quad}));

   progCompute.link(
       ComputeShader(version, {shaders::compute}));

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

   glGenVertexArrays(1, &vao); // create an empty VAO

   //glEnable(GL_DEPTH_TEST);
   //glEnable(GL_CULL_FACE);
   //glEnable(GL_MULTISAMPLE);
}

void RenderWidget::resizeGL(int width, int height)
{
   if (curSize == QSize(-1, -1)) {
      panX = width * 0.75;
      panY = height * 0.5;
      scale = 2.0 / height;
   }
   glViewport(0, 0, width, height);
   curSize = QSize(width, height);
   aspect = (double) width / height;
}

void RenderWidget::createTexture(QSize size)
{
   if (tex) {
      glDeleteTextures(1, &tex);
   }

   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);

   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   // same internal format as compute shader input
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.width(), size.height(),
                0, GL_RGBA, GL_FLOAT, NULL);

}

void RenderWidget::paintGL()
{
   if (curSize != texSize) {
      createTexture(curSize);
      texSize = curSize;
   }

   progCompute.use();

   glUniform2f(progCompute.uniform("center"), panX, panY);
   glUniform1f(progCompute.uniform("scale"), scale);

   glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

   int lsize[3];
   progCompute.computeLocalSize(lsize);

   int ngroups[3];
   ngroups[0] = (texSize.width() + lsize[0]-1) / lsize[0];
   ngroups[1] = (texSize.height() + lsize[1]-1) / lsize[1];
   ngroups[2] = 1;

   glDispatchCompute(ngroups[0], ngroups[1], ngroups[2]);

   // prevent sampling before all writes to image are done
   glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   // display the output of the compute shader
   progQuad.use();

   glUniform1i(progQuad.uniform("sampler"), 0);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex);

   glBindVertexArray(vao);
   glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
       //double cx = (event->x() + scale*panX) / scale;
       //double cy = (event->y() + scale*panY) / scale;

       scale *= pow(1.01, double(deltaY) / 10);

       //panX = (cx - scale*event->x()) / scale;
       //panY = (cy - scale*event->y()) / scale;
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
//    position(2) *= pow(1.04, (float) -event->delta() / 120);
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

