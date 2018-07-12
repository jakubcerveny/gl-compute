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

#include "mandelbrot.hpp"

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


#include "shaders/quad.glsl.hpp"
#include "shaders/palette.glsl.hpp"
#include "shaders/mandelbrot.glsl.hpp"

void RenderWidget::compileShaders()
{
   const int version = 430;

   progQuad.link(
       VertexShader(version, {shaders::quad}),
       FragmentShader(version, {shaders::quad}));

   progCompute.link(
       ComputeShader(version, {shaders::palette, shaders::mandelbrot}));

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

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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
   progCompute.localSize(lsize);

   int ngroups[3];
   ngroups[0] = (texSize.width() + lsize[0]-1) / lsize[0];
   ngroups[1] = (texSize.height() + lsize[1]-1) / lsize[1];
   ngroups[2] = 1;

   glDispatchCompute(ngroups[0], ngroups[1], ngroups[2]);

   // prevent sampling before all writes to texture are done
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
   centerPos = QPoint(lastPos.x(), curSize.height() - lastPos.y());
}

void RenderWidget::zoom(double delta)
{
   double cx = scale*(centerPos.x() - panX);
   double cy = scale*(centerPos.y() - panY);

   scale *= pow(1.01, delta);

   panX = (scale*centerPos.x() - cx) / scale;
   panY = (scale*centerPos.y() - cy) / scale;
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
   double deltaX(event->x() - lastPos.x());
   double deltaY(event->y() - lastPos.y());

   bool leftButton();
   bool rightButton(event->buttons() & Qt::RightButton);

   if (event->buttons() & Qt::RightButton)
   {
      zoom(deltaY/2);
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
   centerPos = QPoint(event->x(), curSize.height() - event->y());
   zoom(-double(event->delta()) / 10);
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
    setWindowTitle("mandelbrot");
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

    QSize size(1200, 1000);
    wnd.resize(size);
    wnd.show();

    return app.exec();
}
