#include <fstream>

#include <QApplication>

#include "main.hpp"
#include "render.hpp"


MainWindow::MainWindow(QWidget* gl)
{
    setCentralWidget(gl);
    setWindowTitle("gl-compute");
}


int main(int argc, char *argv[])
{
    /*if (argc < 2) {
        std::cout << "Usage: gl-compute <mesh> <solution>\n";
        return EXIT_FAILURE;
    }*/

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
