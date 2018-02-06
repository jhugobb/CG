#include "mainview.h"
#include "math.h"
#include "vertex.h"

#include <QDateTime>

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));

}

MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
    glDeleteBuffers(3, &vbo);
    glDeleteVertexArrays(3, &vao);
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

    // Defining a triangle
    Vertex vArray[3];
    vArray[0] = Vertex(-1, -1, 1, 0, 0);
    vArray[1] = Vertex(0, 1, 0, 1, 0);
    vArray[2] = Vertex(1, -1, 0, 0, 1);

    // Initializing the shader programs
    p.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
    p.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");
    p.link();
    p.bind();

    // Generating the OpenGL Objects
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    // Sending the triangle to the GPU
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*15, vArray, GL_STATIC_DRAW);

    // Telling the GPU the data has been layed out
    GLsizei size = sizeof(Vertex);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(float)*2));

}

void MainView::resizeGL(int newWidth, int newHeight) {

    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
}

void MainView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
