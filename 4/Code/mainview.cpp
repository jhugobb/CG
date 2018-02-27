#include "mainview.h"
#include "math.h"
#include "vertex.h"
#include "square.h"
#include "cube.h"
#include "pyramid.h"
#include <QDateTime>
#include <QMatrix4x4>

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
    //making the matrix initial transformations

    turnCubeToOriginal();
    turnPyramidToOriginal();
    turnModelToOriginal();
    projMatrix.perspective(60, 1, 0.01, 100);
}

/**
 * @brief MainView::turnCubeToOriginal Translates and scales the cube to its original state
 */
void MainView::turnCubeToOriginal () {
    cubeMatrix = QMatrix4x4();
    cubeMatrix.translate(2, 0, -6);
}

/**
 * @brief MainView::turnCubeToOriginal Translates and scales the pyramid to its original state
 */
void MainView::turnPyramidToOriginal () {
    pyramidMatrix = QMatrix4x4();
    pyramidMatrix.translate(-2, 0, -6);
}

/**
 * @brief MainView::turnModelToOriginal Translates and scales the model to its original state
 * @param m Model
 */
void MainView::turnModelToOriginal () {
    modelMatrix = QMatrix4x4();
    modelMatrix.translate(0, 0, -10);
    modelMatrix.scale(modelScale);
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

    //cleaning everything
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    shaderProgram[ShadingMode::NORMAL].removeAllShaders();
    shaderProgram[ShadingMode::NORMAL].release();
    shaderProgram[ShadingMode::PHONG].removeAllShaders();
    shaderProgram[ShadingMode::PHONG].release();
    shaderProgram[ShadingMode::GOURAUD].removeAllShaders();
    shaderProgram[ShadingMode::GOURAUD].release();
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

    // Generating the OpenGL Objects
    glGenBuffers(3, vbo);
    glGenVertexArrays(3, vao);

    GLsizei size = sizeof(Vertex);

    /*
    //Drawing the cube
    //creating 8 vertices for the cube
    Vertex v[8];
    v[0] = Vertex(-1, -1, 1, 1, 0, 0);
    v[1] = Vertex(1, -1, 1, 0, 1, 0);
    v[2] = Vertex(1, 1, 1, 0, 0, 1);
    v[3] = Vertex(-1, 1, 1, 1, 1, 0);
    v[4] = Vertex(-1, -1, -1, 1, 0, 1);
    v[5] = Vertex(-1, 1, -1, 0, 1, 1);
    v[6] = Vertex(1, 1, -1, 1, 0, 1);
    v[7] = Vertex(1, -1, -1, 1, 1, 1);
    Cube cube = Cube(v);

    // Sending the cube to the GPU (filling the vbo)
    Vertex c[36];
    cube.toVArray(c);

    glBindVertexArray(vao[MODELINDEX::CUBE]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[MODELINDEX::CUBE]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), c, GL_DYNAMIC_DRAW);

    // Telling the GPU how the data has been layed out

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(float)*3));


    //Creating the pyramid
    Vertex v2[5];
    v2[0] = Vertex(0, 1, 0, 1, 1, 1);
    v2[1] = Vertex(-1, -1, 1, 1, 0, 0);
    v2[2] = Vertex(1, -1, 1, 0, 1, 0);
    v2[3] = Vertex(1, -1, -1, 0, 0, 1);
    v2[4] = Vertex(-1, -1, -1, 1, 0, 1);

    Pyramid pyr = Pyramid(v2);

    //Sending the pyramid to the gpu
    Vertex p[18];
    pyr.toVArray(p);

    glBindVertexArray(vao[MODELINDEX::PYRAMID]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[MODELINDEX::PYRAMID]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), p, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(GLfloat)*3));
    */

    Model m = Model(":/models/cat.obj");
    m.unitize();

    QVector<QVector3D> vm = m.getVertices();
    modelSize = vm.size();
    Vertex vv[modelSize];
    QVector<QVector3D> normals = m.getNormals();
    for (int i = 0 ; i < vm.size() ; i++) {
        vv[i] = Vertex(vm[i].x(), vm[i].y(), vm[i].z(), normals[i].x(), normals[i].y(), normals[i].z());
    }

    //Buffering the model
    glBindVertexArray(vao[MODELINDEX::MODEL]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[MODELINDEX::MODEL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * modelSize, vv, GL_DYNAMIC_DRAW);

    //Sending layout info
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(GLfloat)*3));
}

void MainView::createShaderProgram()
{
    //NORMAL
    shaderProgram[ShadingMode::NORMAL].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    shaderProgram[ShadingMode::NORMAL].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    shaderProgram[ShadingMode::NORMAL].link();

    modelShaderTransform[ShadingMode::NORMAL] = shaderProgram[ShadingMode::NORMAL].uniformLocation("modelTransform");
    projLocation[ShadingMode::NORMAL] = shaderProgram[ShadingMode::NORMAL].uniformLocation("projTransform");
    normalLocation[ShadingMode::NORMAL] = shaderProgram[ShadingMode::NORMAL].uniformLocation("normalTransform");
    lightPosition[ShadingMode::NORMAL] = shaderProgram[ShadingMode::NORMAL].uniformLocation("lightPosition");

    //PHONG

    shaderProgram[ShadingMode::PHONG].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    shaderProgram[ShadingMode::PHONG].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    shaderProgram[ShadingMode::PHONG].link();

    modelShaderTransform[ShadingMode::PHONG]= shaderProgram[ShadingMode::PHONG].uniformLocation("modelTransform");
    projLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("projTransform");
    normalLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("normalTransform");
    lightPosition[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("lightPosition");

    //GOURAUD

    shaderProgram[ShadingMode::GOURAUD].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    shaderProgram[ShadingMode::GOURAUD].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    shaderProgram[ShadingMode::GOURAUD].link();

    modelShaderTransform[ShadingMode::GOURAUD]= shaderProgram[ShadingMode::GOURAUD].uniformLocation("modelTransform");
    projLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("projTransform");
    normalLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("normalTransform");
    lightPosition[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("lightPosition");

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
    double light[3] = {20, 60, 150};

    shaderProgram[currentShade].bind();

    glUniformMatrix4fv(projLocation[currentShade], 1, GL_FALSE, (GLfloat *) projMatrix.data());
    glUniformMatrix3fv(normalLocation[currentShade], 1, GL_FALSE, (GLfloat *) modelMatrix.normalMatrix().data());
    glUniform3fv(lightPosition[currentShade], 1, (GLfloat *) light);

    //draw

    //cube
    glBindVertexArray(vao[MODELINDEX::CUBE]);
    glUniformMatrix4fv(modelShaderTransform[currentShade], 1, GL_FALSE, (GLfloat *) cubeMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //pyramid
    glBindVertexArray(vao[MODELINDEX::PYRAMID]);
    glUniformMatrix4fv(modelShaderTransform[currentShade], 1, GL_FALSE, (GLfloat *) pyramidMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, 18);

    //model
    glBindVertexArray(vao[MODELINDEX::MODEL]);
    glUniformMatrix4fv(modelShaderTransform[currentShade], 1, GL_FALSE, (GLfloat *) modelMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, modelSize);

    shaderProgram[currentShade].release();
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
    float ratio = ((float) newWidth) / ((float) newHeight);

    projMatrix.setToIdentity();
    projMatrix.perspective(60, ratio, 0.01, 100);
    update();

}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";

    //reinitialization of the matrixes so that the rotation is effectively correct
    turnCubeToOriginal();
    turnPyramidToOriginal();
    turnModelToOriginal();

    //qreal x = ((qreal) rotateX) / 360;
    xRotation = rotateX;
    yRotation = rotateY;
    zRotation = rotateZ;

    cubeMatrix.scale(scale);
    pyramidMatrix.scale(scale);
    modelMatrix.scale(scale);
    rotate(xRotation, yRotation, zRotation);
    update();
}

/**
 * @brief MainView::rotate Auxiliary function to define the rotation of the models.
 * @param x X axis rotation.
 * @param y Y axis rotation.
 * @param z Z axis rotation.
 */
void MainView::rotate(qreal x, qreal y, qreal z) {
    cubeMatrix.rotate(x, 1, 0, 0);
    cubeMatrix.rotate(y, 0, 1, 0);
    cubeMatrix.rotate(z, 0, 0, 1);
    pyramidMatrix.rotate(x, 1, 0, 0);
    pyramidMatrix.rotate(y, 0, 1, 0);
    pyramidMatrix.rotate(z, 0, 0, 1);
    modelMatrix.rotate(x, 1, 0, 0);
    modelMatrix.rotate(y, 0, 1, 0);
    modelMatrix.rotate(z, 0, 0, 1);
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;

    qreal s = (qreal) scale;

    s = s / 100.0;
    this->scale = s;

    turnCubeToOriginal();
    turnPyramidToOriginal();
    turnModelToOriginal();
    rotate(xRotation, yRotation, zRotation);
    cubeMatrix.scale(s);
    pyramidMatrix.scale(s);
    modelMatrix.scale(s);
    update();
}

void MainView::setShadingMode(ShadingMode shading)
{
    currentShade = shading;
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

QVector<quint8> MainView::imageToBytes(QImage image) {
    // needed since (0,0) is bottom left in OpenGL
    QImage im = image.mirrored();
    QVector<quint8> pixelData;
    pixelData.reserve(im.width()*im.height()*4);

    for (int i = 0; i != im.height(); ++i) {
        for (int j = 0; j != im.width(); ++j) {
            QRgb pixel = im.pixel(j,i);

            // pixel is of format #AARRGGBB (in hexadecimal notation)
            // so with bitshifting and binary AND you can get
            // the values of the different components
            quint8 r = (quint8)((pixel >> 16) & 0xFF); // Red component
            quint8 g = (quint8)((pixel >> 8) & 0xFF); // Green component
            quint8 b = (quint8)(pixel & 0xFF); // Blue component
            quint8 a = (quint8)((pixel >> 24) & 0xFF); // Alpha component

            // Add them to the Vector
            pixelData.append(r);
            pixelData.append(g);
            pixelData.append(b);
            pixelData.append(a);
        }
    }
    return pixelData;
}
