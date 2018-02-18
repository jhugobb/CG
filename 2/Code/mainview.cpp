#include "mainview.h"
#include "math.h"
#include "vertex.h"
#include "square.h"
#include "cube.h"
#include "pyramid.h"
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

    //making the matrix initial transformations
    cubeMatrix.translate(-2, 0 , -6);
    pyramidMatrix.translate(2, 0, -6);
    projMatrix.perspective(60, 800.0/600.0, 5, -30);

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
    glGenBuffers(2, vbo);
    glGenVertexArrays(2, vao);


    //Drawing the cube
    //creating 8 vertices for the cube
    Vertex v[8];
    v[0] = Vertex(1, 1, -1, 1, 0, 0);
    v[1] = Vertex(-1, 1, -1, 0, 0, 1);
    v[2] = Vertex(-1, -1, -1, 0, 1, 0);
    v[3] = Vertex(1, -1, -1, 1, 0, 0);
    v[4] = Vertex(1, 1, 1, 1, 0, 0);
    v[5] = Vertex(-1, 1, 1, 0, 0, 1);
    v[6] = Vertex(-1, -1, 1, 0, 1, 0);
    v[7] = Vertex(1, -1, 1, 1, 0, 0);
    Cube cube = Cube(v);

    // Sending the cube to the GPU (filling the vbo)
    Vertex c[36];
    cube.toVArray(c); //DO NOT FORGET TO DESTROY

    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), c, GL_DYNAMIC_DRAW);

    // Telling the GPU how the data has been layed out
    GLsizei size = sizeof(Vertex);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(float)*3));


    //Creating the pyramid
    Vertex v2[5];
    v2[0] = Vertex(0, 1, 0, 0, 0, 1);
    v2[1] = Vertex(1, 1, 1, 0, 1, 0);
    v2[2] = Vertex(-1, 1, 1, 1, 0, 0);
    v2[3] = Vertex(1, 1, -1, 1, 0, 0);
    v2[4] = Vertex(-1, 1, -1, 0, 1, 0);

    Pyramid pyr = Pyramid(v2);

    //Sending the pyramid to the gpu
    Vertex p[18];
    pyr.toVArray(p);

    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), p, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(float)*3));

    /*for(int i = 0 ; i < 36 ; i++){
        printf("(%f, %f, %f) (%f, %f, %f)\n", c[i].coord[0], c[i].coord[1], c[i].coord[2], c[i].color[0], c[i].color[1], c[i].color[2]);
    }*/
}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();

    cubeLocation = shaderProgram.uniformLocation("cubeTransform");
    pyramidLocation = shaderProgram.uniformLocation("pyramidTransform");
    projLocation = shaderProgram.uniformLocation("projTransform");
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

    //Sending the matrixes to the shader
    glUniformMatrix4fv(cubeLocation, 1, GL_FALSE, (GLfloat *) cubeMatrix.data());
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, (GLfloat *) projMatrix.data());


    glDrawArrays(GL_TRIANGLES, 0, 36);

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
    // TODO: Update projection to fit the new aspect ratio
    //identity?
    projMatrix.setToIdentity();
    projMatrix.perspective(60, newWidth / newHeight, 5, -30);
    update();

}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    Q_UNIMPLEMENTED();
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;
    Q_UNIMPLEMENTED();
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