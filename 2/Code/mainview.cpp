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

    originCubeM.translate(2, 0 , -6);
    originPyramidM.translate(-2, 0, -6);
    originModelM.translate(0, 0, -6);
    cubeMatrix = QMatrix4x4(originCubeM);
    pyramidMatrix = QMatrix4x4(originPyramidM);
    modelMatrix = QMatrix4x4(originModelM);
    projMatrix.perspective(60, 1, -1, 1);
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

/**
 * @brief getXLength Calculates the length of the element (assuming the vertexes composes an element) from the X axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return X Length
 */
float getXLength (Vertex *v, int N) {
    float min = std::numeric_limits<float>::max(), max = - std::numeric_limits<float>::max();
    for(int i = 0 ; i < N ; i++) {
        if (v[i].coord[0] < min) {
            min = v[i].coord[0];
        }
        else if (v[i].coord[0] > max) {
            max = v[i].coord[0];
        }

    }
    return (max - min);
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
    v2[0] = Vertex(0, 1, 0, 1, 1, 1);
    v2[1] = Vertex(-1, -1, 1, 1, 0, 0);
    v2[2] = Vertex(1, -1, 1, 0, 1, 0);
    v2[3] = Vertex(1, -1, -1, 0, 0, 1);
    v2[4] = Vertex(-1, -1, -1, 1, 0, 1);

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
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(GLfloat)*3));

    Model m = Model(":/models/sphere.obj");

    QVector<QVector3D> vm = m.getVertices();
    modelSize = vm.size();
    Vertex vv[modelSize];
    int i = 0;
    float r,g,b;
    for (QVector3D vert : vm) {
        //Random values calculator for the colours
        r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        vv[i] = Vertex(vert.x(), vert.y(), vert.z(), r, g, b);
        i++;
    }

    //Scaling of the imported model in order for it to have the same lenght (with the X axis as reference) as the the cube and the pyramide (i.e.: 2)
    originModelM.scale(1.0 / getXLength(vv, modelSize));
    modelMatrix = QMatrix4x4(originModelM);

    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * modelSize, vv, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, size, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, size, (GLvoid *) (sizeof(GLfloat)*3));
}

void MainView::createShaderProgram()
{
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();

    modelShaderTransform = shaderProgram.uniformLocation("modelTransform");
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

    glUniformMatrix4fv(projLocation, 1, GL_FALSE, (GLfloat *) projMatrix.data());

    //draw

    //cube
    glBindVertexArray(vao[0]);
    glUniformMatrix4fv(modelShaderTransform, 1, GL_FALSE, (GLfloat *) cubeMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //pyramid
    glBindVertexArray(vao[1]);
    glUniformMatrix4fv(modelShaderTransform, 1, GL_FALSE, (GLfloat *) pyramidMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, 18);

    //model
    glBindVertexArray(vao[2]);
    glUniformMatrix4fv(modelShaderTransform, 1, GL_FALSE, (GLfloat *) modelMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, modelSize);

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
    projMatrix.perspective(60, newWidth / newHeight, -1, 1);
    update();

}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";

    //reinitialization of the matrixes so that the rotation is effectively correct
    cubeMatrix = QMatrix4x4(originCubeM);
    pyramidMatrix = QMatrix4x4(originPyramidM);
    modelMatrix = QMatrix4x4(originModelM);

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

    printf("scaling by %f\n", s);

    cubeMatrix = QMatrix4x4(originCubeM);
    pyramidMatrix = QMatrix4x4(originPyramidM);
    modelMatrix = QMatrix4x4(originModelM);
    rotate(xRotation, yRotation, zRotation);
    cubeMatrix.scale(s);
    pyramidMatrix.scale(s);
    modelMatrix.scale(s);
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
