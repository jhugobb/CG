#include "mainview.h"
#include "math.h"
#include "vertex.h"

#include <QDateTime>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));

    matrix[0] = QMatrix4x4();
    matrix[1] = QMatrix4x4();
    matrix[2] = QMatrix4x4();
    matrix[3] = QMatrix4x4();

    matrix[0].translate(2, 0, -6);
    matrix[1].translate(-2, 0, -6);
    matrix[2].translate(0, 0, -12);

    matrix[3].perspective(60, 800.0/600.0, 5, -30);
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";
    glDeleteBuffers(3, vbo);
    glDeleteVertexArrays(3, vao);
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color of the screen to be black on clear (new frame)
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    // Defining the vertexes of the cube
    Vertex cube[8];
    cube[0] = Vertex(-1, 1, 1, 1, 0, 0);
    cube[1] = Vertex(-1, -1, 1, 0, 1, 0);
    cube[2] = Vertex(1, -1, 1, 0, 0, 1);
    cube[3] = Vertex(1, 1, 1, 0, 1, 0);
    cube[4] = Vertex(-1, 1, -1, 1, 0, 0);
    cube[5] = Vertex(-1, -1, -1, 0, 0, 1);
    cube[6] = Vertex(1, -1, -1, 0, 1, 0);
    cube[7] = Vertex(1, 1, -1, 1, 0, 0);

    // Defining the triangles to be drawn
    tCube[0] = cube[0];
    tCube[1] = cube[1];
    tCube[2] = cube[3];

    tCube[3] = cube[1];
    tCube[4] = cube[2];
    tCube[5] = cube[3];

    tCube[6] = cube[3];
    tCube[7] = cube[2];
    tCube[8] = cube[7];

    tCube[9] = cube[2];
    tCube[10] = cube[6];
    tCube[11] = cube[7];

    tCube[12] = cube[4];
    tCube[13] = cube[0];
    tCube[14] = cube[7];

    tCube[15] = cube[0];
    tCube[16] = cube[3];
    tCube[17] = cube[7];

    tCube[18] = cube[0];
    tCube[19] = cube[4];
    tCube[20] = cube[1];

    tCube[21] = cube[5];
    tCube[22] = cube[1];
    tCube[23] = cube[4];

    tCube[24] = cube[4];
    tCube[25] = cube[5];
    tCube[26] = cube[7];

    tCube[27] = cube[5];
    tCube[28] = cube[6];
    tCube[29] = cube[7];

    tCube[30] = cube[1];
    tCube[31] = cube[2];
    tCube[32] = cube[6];

    tCube[33] = cube[5];
    tCube[34] = cube[1];
    tCube[35] = cube[6];

    // Defining the vertexes of the pyramid
    Vertex pyramid[5];
    pyramid[0] = Vertex(-1, -1, 1, 1, 0, 0);
    pyramid[1] = Vertex(-1, -1, -1, 0, 1, 0);
    pyramid[2] = Vertex(1, -1, -1, 0, 0, 1);
    pyramid[3] = Vertex(1, -1, 1, 1, 0, 0);
    pyramid[4] = Vertex(0, 1, 0, 0, 1, 1);

    // Defining the triangles to be drawn
    tPyramid[0] = pyramid[0];
    tPyramid[1] = pyramid[2];
    tPyramid[2] = pyramid[1];

    tPyramid[3] = pyramid[0];
    tPyramid[4] = pyramid[3];
    tPyramid[5] = pyramid[2];

    tPyramid[6] = pyramid[0];
    tPyramid[7] = pyramid[3];
    tPyramid[8] = pyramid[4];

    tPyramid[9] = pyramid[3];
    tPyramid[10] = pyramid[2];
    tPyramid[11] = pyramid[4];

    tPyramid[12] = pyramid[2];
    tPyramid[13] = pyramid[1];
    tPyramid[14] = pyramid[4];

    tPyramid[15] = pyramid[1];
    tPyramid[16] = pyramid[0];
    tPyramid[17] = pyramid[4];

    // Defining the model
    Model model = Model(":/models/cat.obj");

    QVector<QVector3D> vertexes = model.getVertices();
    nbrOfModelVertexes = vertexes.size();
    qDebug() << ":: Number or vertexes:" << nbrOfModelVertexes;
    Vertex tModel[nbrOfModelVertexes];

    int i = 0;
    srand(time(0));
    for (QVector<QVector3D>::iterator it = vertexes.begin(); it != vertexes.end(); ++it) {
        QVector3D point = *it;
        Vertex v = Vertex(point.x(), point.y(), point.z(), rand(), rand(), rand());
        tModel[i] = v;
        i++;
    }

    // Creating the VBOs and the VAOs
    glGenBuffers(3, vbo);
    glGenVertexArrays(3, vao);

    // Filling the VBO with the cube  data
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 216, tCube, GL_DYNAMIC_DRAW);

    GLsizei size = sizeof(Vertex);

    // Telling the GPU the data has been layed out
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(GLfloat)*3));

    // Filling the VBO with the pyramid data
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, tPyramid, GL_DYNAMIC_DRAW);

    // Telling the GPU the data has been layed out
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(float)*3));


    // Filling the VBO with the pyramid data
    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * nbrOfModelVertexes * 6), tModel, GL_DYNAMIC_DRAW);

    // Telling the GPU the data has been layed out
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(float)*3));

}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    glUniformMatrix4fv(shaderProgram.uniformLocation("modelTransform"), 1, GL_FALSE, (GLfloat *) matrix[0].data());
    glUniformMatrix4fv(shaderProgram.uniformLocation("modelPerspective"), 1, GL_FALSE, (GLfloat *) matrix[3].data());

    // Draw here

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(vao[1]);
    glUniformMatrix4fv(shaderProgram.uniformLocation("modelTransform"), 1, GL_FALSE, (GLfloat *) matrix[1].data());
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glBindVertexArray(vao[2]);
    glUniformMatrix4fv(shaderProgram.uniformLocation("modelTransform"), 1, GL_FALSE, (GLfloat *) matrix[2].data());
    glDrawArrays(GL_TRIANGLES, 0, nbrOfModelVertexes);

    shaderProgram.release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) 
{
    matrix[3] = QMatrix4x4();
    matrix[3].perspective(60, newWidth/newHeight, 5, -30);
    update();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    if (!rotateX && !rotateY && !rotateZ) {
        matrix[0] = QMatrix4x4();
        matrix[1] = QMatrix4x4();
        matrix[2] = QMatrix4x4();
        matrix[0].translate(2, 0, -6);
        matrix[1].translate(-2, 0, -6);
        matrix[2].translate(0, 0, -12);
    }
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    matrix[0].rotate(10, rotateX, rotateY, rotateZ);
    matrix[1].rotate(10, rotateX, rotateY, rotateZ);
    matrix[2].rotate(10, rotateX, rotateY, rotateZ);
    update();
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;

    // Back to normal
    matrix[0] = QMatrix4x4();
    matrix[1] = QMatrix4x4();
    matrix[2] = QMatrix4x4();
    matrix[0].translate(2, 0, -6);
    matrix[1].translate(-2, 0, -6);
    matrix[2].translate(0, 0, -12);

    // Applying the scale value
    if(scale != 100) {
        matrix[0].scale(0.01 * scale);
        matrix[1].scale(0.01 * scale);
        matrix[2].scale(0.01 * scale);
    }
    update();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
