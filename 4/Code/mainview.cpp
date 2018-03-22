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
 * @brief MainView::loadTexture
 *
 * Loads the texture in the path "file"
 */
void MainView::loadTexture(QString file, GLuint texturePtr) {
    GLfloat f;
    QVector<quint8> img = imageToBytes(QImage(file));
    glBindTexture(GL_TEXTURE_2D, texturePtr);

    // Anisotropic Filtering
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
    glGenerateMipmap(GL_TEXTURE_2D);

}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Used to clean up variables, buffers etc.
 *
 */
MainView::~MainView() {
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";

    //cleaning everything
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    shaderProgram[NORMAL].removeAllShaders();
    shaderProgram[NORMAL].release();
    shaderProgram[PHONG].removeAllShaders();
    shaderProgram[PHONG].release();
    shaderProgram[GOURAUD].removeAllShaders();
    shaderProgram[GOURAUD].release();
    glDeleteBuffers(COUNT, vbo);
    glDeleteVertexArrays(COUNT, vao);
    glDeleteTextures(COUNT, texture);
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
    glGenBuffers(COUNT, vbo);
    glGenVertexArrays(COUNT, vao);
    glGenTextures(COUNT, texture);

    GLsizei size = sizeof(Vertex);

    Model m = Model(":/models/cat.obj");
    m.unitize();
    loadTexture(":/textures/cat_diff.png", texture[MODEL]);
    QVector<QVector3D> vm = m.getVertices();
    modelSize = vm.size();
    Vertex vv[modelSize];
    QVector<QVector3D> normals = m.getNormals();
        QVector<QVector2D> texCoords = m.getTextureCoords();
    for (int i = 0 ; i < vm.size() ; i++) {
        vv[i] = Vertex(vm[i].x(), vm[i].y(), vm[i].z(), normals[i].x(), normals[i].y(), normals[i].z(), texCoords[i].x(), texCoords[i].y());
    }



    //Buffering the model
    glBindVertexArray(vao[MODEL]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[MODEL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * modelSize, vv, GL_DYNAMIC_DRAW);

    //Sending layout info
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(GLfloat)*3));
    glVertexAttribPointer(2, 2, GL_FLOAT, false, size, (GLvoid *) (sizeof(GLfloat)*6));
}

void MainView::createShaderProgram()
{
    //NORMAL
    shaderProgram[NORMAL].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    shaderProgram[NORMAL].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    shaderProgram[NORMAL].link();

    modelShaderTransform[NORMAL] = shaderProgram[NORMAL].uniformLocation("modelTransform");
    projLocation[NORMAL] = shaderProgram[NORMAL].uniformLocation("projTransform");
    normalLocation[NORMAL] = shaderProgram[NORMAL].uniformLocation("normalTransform");

    //PHONG

    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    shaderProgram[PHONG].link();

    modelShaderTransform[PHONG]= shaderProgram[PHONG].uniformLocation("modelTransform");
    projLocation[PHONG] = shaderProgram[PHONG].uniformLocation("projTransform");
    normalLocation[PHONG] = shaderProgram[PHONG].uniformLocation("normalTransform");
    samplerLocation[0] = shaderProgram[GOURAUD].uniformLocation("samplerUniform");

    //GOURAUD

    shaderProgram[GOURAUD].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    shaderProgram[GOURAUD].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    shaderProgram[GOURAUD].link();

    modelShaderTransform[GOURAUD]= shaderProgram[GOURAUD].uniformLocation("modelTransform");
    projLocation[GOURAUD] = shaderProgram[GOURAUD].uniformLocation("projTransform");
    normalLocation[GOURAUD] = shaderProgram[GOURAUD].uniformLocation("normalTransform");
    samplerLocation[1] = shaderProgram[GOURAUD].uniformLocation("samplerUniform");

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

    shaderProgram[currentShade].bind();

    glUniformMatrix4fv(projLocation[currentShade], 1, GL_FALSE, (GLfloat *) projMatrix.data());
    glUniformMatrix3fv(normalLocation[currentShade], 1, GL_FALSE, (GLfloat *) modelMatrix.normalMatrix().data());

    //draw

    //model

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[MODEL]);
    if(currentShade == PHONG)
        glUniform1i(samplerLocation[0], 0);
    if(currentShade == GOURAUD)
        glUniform1i(samplerLocation[1], 0);

    glBindVertexArray(vao[MODEL]);
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
