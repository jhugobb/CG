#include "mainview.h"
#include "math.h"
#include "vertex.h"
#include <QOpenGLShaderProgram>

#include <QDateTime>

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
}

void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    //Code here
    Vertex v[3];
    v[0] = new vertex(-0.9, -0.9, 0,1, 0);
    v[1] = new vertex(0.9, -0.9, 1, 0, 0);
    v[2] = new vertex(0, 0.9, 0, 0, 1);
    //END

    if(p.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl"))
        printf("SUCCESS\n");

}

void MainView::resizeGL(int newWidth, int newHeight) {

    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

void MainView::paintGL() {

}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
