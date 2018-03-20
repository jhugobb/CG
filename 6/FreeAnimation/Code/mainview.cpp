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
}

/**
 * @brief MainView::updateProjectionMatrix Places the eye on the determined spherical coordinates
 */
void MainView::updateProjectionMatrix()
{
    projMatrix.setToIdentity();
    projMatrix.perspective(60, projectionRatio, 0.01, 100);

    qreal azimuth = (M_PI / 180.0) * perspectiveRotation; //calculates the azimuth value in radians
    qreal inclination = (M_PI / 180.0) * perspectiveHeight; //calculates the inclination value in radians

    qreal z = perspectiveDistance * sin(inclination) * cos(azimuth); //calcules the z coordinate of the eye
    qreal x = perspectiveDistance * sin(inclination) * sin(azimuth);//calcules the x coordinate of the eye
    qreal y = perspectiveDistance * cos(inclination);//calcules the y coordinate of the eye

    projMatrix.rotate(-perspectiveRotation, 0, 1, 0); //rotates the eye to direction of the focus point (currently (0,0,0)) from the perspective of the XoZ plane

    QVector3D perp = QVector3D(z, 0, -x).normalized(); //calculates a vector that is perpendicular to the ray view->focus_point, from the perspective of the plane XoZ

    projMatrix.rotate(90 - perspectiveHeight, perp.x(), perp.y(), perp.z()); //rotates the eye to point to the focus point.

    projMatrix.translate(-x, -y, -z); //Symetrics because projections translate to the symetric (XoYoZ) position
}

void MainView::updateObjects ()
{
    for (int i = 0 ; i < MODELINDEX::COUNT ; i++)
    {
        objectMatrix[i].setToIdentity();
        objectMatrix[i].translate(xTranslation[i], yTranslation[i], zTranslation[i]);
        objectMatrix[i].scale(generalScale);
        objectMatrix[i].scale(objectScale[i]);
        objectMatrix[i].rotate(xGeneralRotation, 1, 0, 0);
        objectMatrix[i].rotate(yGeneralRotation, 0, 1, 0);
        objectMatrix[i].rotate(zGeneralRotation, 0, 0, 1);
        objectMatrix[i].rotate(xRotation[i], 1, 0, 0);
        objectMatrix[i].rotate(yRotation[i], 0, 1, 0);
        objectMatrix[i].rotate(zRotation[i], 0, 0, 1);
    }
}

/**
 * @brief MainView::loadTexture
 *
 * Loads the texture in the path "file"
 */
void MainView::loadTexture(QString file, GLuint texturePtr) {
    GLfloat f;
    QImage qimg = QImage(file);
    QVector<quint8> img = imageToBytes(qimg);
    glBindTexture(GL_TEXTURE_2D, texturePtr);

    // Anisotropic Filtering
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, qimg.width(), qimg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());
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
    for (GLuint i = 0 ; i < COUNTSHADER ; i++)
    {
        shaderProgram[i].removeAllShaders();
        shaderProgram[i].release();
    }
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

    // Loading the models
    loadModel(JUPITER, ":/models/sphere.obj", ":/textures/jupitermap.jpg");
    loadModel(MOON1, ":/models/sphere.obj", ":/textures/mars_1k_color.jpg");
    loadModel(MOON2, ":/models/sphere.obj", ":/textures/mercurymap.jpg");
    loadModel(MOON3, ":/models/sphere.obj", ":/textures/earthmap1k.png");
    loadModel(SURFACE, ":/models/grid.obj", ":/textures/rug_logo.png");
    loadModel(CAT, ":/models/cat.obj", ":/textures/cat_diff.png");

    initializeObjectsAttributes();

    timer.start(FPS);
}



/**
 * @brief MainView::initializeObjectsAttributes
 *
 * Initializes the attributes of the models
 */
void MainView::initializeObjectsAttributes()
{
    for (int i = 0 ; i < COUNT ; i++)
    {
        xRotation[i] = 0;
        yRotation[i] = 0;
        zRotation[i] = 0;
        xTranslation[i] = 0;
        yTranslation[i] = 0;
        zTranslation[i] = 0;
        objectScale[i] = 1;
        ti[i] = 0;
    }

    distanceToJupiter[JUPITER] = 0;
    distanceToJupiter[MOON1] = 3;
    distanceToJupiter[MOON2] = 4;
    distanceToJupiter[MOON3] = 1.5;

    xTranslation[JUPITER] = 0;
    yTranslation[JUPITER] = 6;

    xTranslation[MOON1] = distanceToJupiter[MOON1];
    objectScale[MOON1] = 0.2;

    yTranslation[MOON2] = distanceToJupiter[MOON2];
    objectScale[MOON2] = 0.3;

    xTranslation[MOON3] = distanceToJupiter[MOON3];
    objectScale[MOON3] = 0.1;

    xRotation[SURFACE] = -90;
    objectScale[SURFACE] = 10;


}

void MainView::loadModel(MODELINDEX modelNr,  char const *objPath, char const *texturePath)
{
    Model m = Model(objPath);
    m.unitize();
    loadTexture(texturePath, texture[modelNr]);
    QVector<QVector3D> vm = m.getVertices();
    modelSize[modelNr] = vm.size();
    Vertex vv[modelSize[modelNr]];
    QVector<QVector3D> normals = m.getNormals();
    QVector<QVector2D> texCoords = m.getTextureCoords();
    for (int i = 0 ; i < vm.size() ; i++) {
        vv[i] = Vertex(vm[i].x(), vm[i].y(), vm[i].z(), normals[i].x(), normals[i].y(), normals[i].z(), texCoords[i].x(), texCoords[i].y());
    }

    //Buffering the model
    glBindVertexArray(vao[modelNr]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[modelNr]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * modelSize[modelNr], vv, GL_DYNAMIC_DRAW);

    //Sending layout info
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *) (sizeof(GLfloat)*3));
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid *) (sizeof(GLfloat)*6));
}

void MainView::createShaderProgram()
{
    //NORMAL
    shaderProgram[NORMAL].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                  ":/shaders/vertshader_normal.glsl");
    shaderProgram[NORMAL].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                  ":/shaders/fragshader_normal.glsl");
    //PHONG

    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                 ":/shaders/vertshader_phong.glsl");
    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                 ":/shaders/fragshader_phong.glsl");

    //GOURAUD

    shaderProgram[GOURAUD].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                   ":/shaders/vertshader_gouraud.glsl");
    shaderProgram[GOURAUD].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                   ":/shaders/fragshader_gouraud.glsl");

    for (GLuint i = 0 ; i < COUNTSHADER ; i++)
    {
        shaderProgram[i].link();

        modelShaderTransform[i]= shaderProgram[i].uniformLocation("modelTransform");
        projLocation[i] = shaderProgram[i].uniformLocation("projTransform");
        normalLocation[i] = shaderProgram[i].uniformLocation("normalTransform");
        samplerLocation[i] = shaderProgram[i].uniformLocation("samplerUniform");
        lightColorLocation[i] = shaderProgram[i].uniformLocation("lightColor");
        materialColorLocation[i] = shaderProgram[i].uniformLocation("materialColor");
        lightPositionLocation[i] = shaderProgram[i].uniformLocation("lightPosition");
        materialLocation[i] = shaderProgram[i].uniformLocation("material");
    }
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

    //Animation

    animate();

    updateProjectionMatrix();
    updateObjects();

    float lightColor[3] = {1.0, 1.0, 1.0};
    float materialColor[3] = {1.0, 1.0, 1.0};
    float material[4] = {0.2, 0.8, 0.0, 1};
    float lightPosition[3] = {100.0, 100.0, 150.0};

    shaderProgram[currentShade].bind();

    glUniformMatrix4fv(projLocation[currentShade], 1, GL_FALSE, (GLfloat *) projMatrix.data());
    glUniform3fv(lightColorLocation[currentShade], 1, lightColor);
    glUniform3fv(materialColorLocation[currentShade], 1, materialColor);
    glUniform3fv(lightPositionLocation[currentShade], 1, lightPosition);
    glUniform4fv(materialLocation[currentShade], 1, material);
    glUniform1i(samplerLocation[currentShade], 0);

    //models

    for (int i = 0 ; i < MODELINDEX::COUNT ; i++)
    {
        glUniformMatrix3fv(normalLocation[currentShade], 1, GL_FALSE, (GLfloat *) objectMatrix[i].normalMatrix().data());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[i]);

        glBindVertexArray(vao[i]);
        glUniformMatrix4fv(modelShaderTransform[currentShade], 1, GL_FALSE, (GLfloat *) objectMatrix[i].data());
        glDrawArrays(GL_TRIANGLES, 0, modelSize[i]);
    }

    shaderProgram[currentShade].release();
}

void MainView::freeFallJump(MODELINDEX jumper, MODELINDEX surface, qreal initialVelocity)
{
    const qreal G = -9.8;

    if (yTranslation[jumper] < yTranslation[surface])
        ti[jumper] = 0;
    ti[jumper] += FPS/1000; //since there's a frame every 1000/60 milisseconds, we should add 1/60 of a second to the time variable at each frame

    yTranslation[jumper] = yTranslation[surface] + initialVelocity * ti[jumper] + 0.5 * G * ti[jumper] * ti[jumper];
}


/**
 * @brief MainView::animate
 *
 * Called every time we want to compute the next frame of the models' animation
 *
 */
void MainView::animate() {
    if (animationIsRunning)
    {
        //Time variable
        ti[JUPITER] += 0.1f;
        ti[MOON1] += 0.1f;
        ti[MOON2] += 0.1f;
        ti[MOON3] += 0.1f;
        // Makes jupiter rotate around itself
        AddRotation(JUPITER, 0, 0.3, 0);
        AddRotation(MOON1, 0.3, 0, 0);
        AddRotation(MOON2, 0, 0.6, 0);
        AddRotation(MOON3, 0, 0, 0.9);

        // variable that determines the step of the orbit

        float angle[3];

        angle[0] = ti[MOON1] * 3.1419f / (FPS);

        angle[1] = 0.03f * ti[MOON1] * 3.1419f / (FPS);

        angle[2] = ti[MOON1] * 3.1419f / (FPS);

        xTranslation[MOON1] = sin(angle[0]) * distanceToJupiter[MOON1] + xTranslation[JUPITER];
        yTranslation[MOON1] = cos(angle[0]) * distanceToJupiter[MOON1] + yTranslation[JUPITER];
        zTranslation[MOON1] = zTranslation[JUPITER];

        xTranslation[MOON2] = sin(angle[1]) * distanceToJupiter[MOON2] + xTranslation[JUPITER];
        yTranslation[MOON2] = cos(angle[1]) * distanceToJupiter[MOON2] + yTranslation[JUPITER];
        zTranslation[MOON2] = cos(angle[1]) * distanceToJupiter[MOON2] + zTranslation[JUPITER];

        xTranslation[MOON3] = sin(angle[2]) * distanceToJupiter[MOON3] + xTranslation[JUPITER];
        yTranslation[MOON3] = yTranslation[JUPITER];
        zTranslation[MOON3] = cos(angle[2]) * distanceToJupiter[MOON3] + zTranslation[JUPITER];

        freeFallJump(CAT, SURFACE, 5);
    }
}


void MainView::AddRotation(int index, qreal x, qreal y, qreal z)
{
    xRotation[index] += x;
    xRotation[index] = fmod(xRotation[index] , 360.0);
    yRotation[index] += y;
    yRotation[index] = fmod(yRotation[index] , 360.0);
    zRotation[index] += z;
    zRotation[index] = fmod(zRotation[index] , 360.0);
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

    projectionRatio = ratio;
    update();

}

// --- Public interface

void MainView::setRotation(qreal rotateX, qreal rotateY, qreal rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";

    //qreal x = ((qreal) rotateX) / 360;
    xGeneralRotation = rotateX;
    yGeneralRotation = rotateY;
    zGeneralRotation = rotateZ;

    update();
}

void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;

    qreal s = (qreal) scale;

    s = s / 100.0;
    generalScale = s;

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
