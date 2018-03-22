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
    viewMatrix.setToIdentity();
    projMatrix.perspective(60, projectionRatio, 0.01, 100);

    if (personMode == THIRD)
    {
        qreal azimuth = (M_PI / 180.0) * perspectiveRotation; //calculates the azimuth value in radians
        qreal inclination = (M_PI / 180.0) * perspectiveHeight; //calculates the inclination value in radians

        qreal x = sokoban.character.x() + perspectiveDistance * sin(inclination) * sin(azimuth);//calcules the x coordinate of the eye
        qreal y = perspectiveDistance * cos(inclination);//calcules the y coordinate of the eye
        qreal z = sokoban.character.y() + perspectiveDistance * sin(inclination) * cos(azimuth); //calcules the z coordinate of the eye

        viewMatrix.rotate(-perspectiveRotation, 0, 1, 0); //rotates the eye to direction of the focus point from the perspective of the XoZ plane

        QVector3D PC = QVector3D(sokoban.character.x(), 0, sokoban.character.y()) - QVector3D(x, y, z); //Calculates the eye-character vector

        QVector3D perp = QVector3D(PC.z(), 0, -PC.x()).normalized(); //calculates a vector that is perpendicular to the ray view->focus_point, from the perspective of the plane XoZ

        viewMatrix.rotate(perspectiveHeight - 90, perp.x(), perp.y(), perp.z()); //rotates the eye to point to the focus point.

        viewMatrix.translate(-x, -y, -z); //Symetrics because projections translate to the symetric (XoYoZ) position
    }
    else if (personMode == FIRST)
    {
        qDebug() << sokoban.character.x() << "z: " << sokoban.character.y();
        viewMatrix.rotate(90 - sokoban.orientation, 0, 1, 0);
        viewMatrix.translate(-sokoban.character.x(), -1, -sokoban.character.y());
        qDebug() << sokoban.orientation - 90;
    }
}

void MainView::updateObjects ()
{
    QMatrix4x4 m = QMatrix4x4();

    m.translate(sokoban.get(CHARACTER).at(0).x(), -0.5, sokoban.get(CHARACTER).at(0).y());
    m.rotate(sokoban.orientation, 0, 1, 0);

    objectMatrixes[CHARACTER].replace(0, m);

    if (sokoban.changedBox != -1)
    {
        m = QMatrix4x4();
        m.translate(sokoban.get(BOXES).at(sokoban.changedBox).x(), 0, sokoban.get(BOXES).at(sokoban.changedBox).y());
        objectMatrixes[BOXES].replace(sokoban.changedBox, m);
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
    glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

    createShaderProgram();

    // Generating the OpenGL Objects
    glGenBuffers(COUNT, vbo);
    glGenVertexArrays(COUNT, vao);
    glGenTextures(COUNT, texture);

    loadModel(WALLS, ":/models/cube.obj", ":/textures/rock.png");
    loadModel(BOXES, ":/models/cube.obj", ":/textures/wood.png");
    loadModel(CHARACTER, ":/models/cat.obj", ":/textures/cat_diff.png");
    loadModel(FLAGS, ":/models/grid.obj", ":/textures/red.png");
    loadModel(SURFACE, ":/models/grid.obj", ":/textures/grass.png");

    loadSokoban();

    initializeObjectsAttributes();

}

void MainView::loadSokoban()
{
    timer.stop();
    animationIsRunning = false;
    QString dir = ":/maps/lvl";
    dir = dir + QString::number(currentLevel);
    dir = dir + ".txt";
    qDebug() << dir;
    sokoban = Sokoban(dir);
    QMatrix4x4 matrix;
    for (int type = 0 ; type < COUNT ; type++) //initializes the matrixes list
    {
        objectMatrixes[type] = QVector<QMatrix4x4>();

        for (int index = 0 ; index < sokoban.get(type).size() ; index++)
        {
            matrix = QMatrix4x4();
            objectMatrixes[type].append(matrix);
        }
    }

    objectMatrixes[SURFACE].append(QMatrix());
}

void MainView::initializeObjectsAttributes()
{
    QMatrix4x4 m;
    for (int i = 0 ; i < MODELINDEX::COUNT ; i++)
    {
        for (int objInd = 0 ; objInd < objectMatrixes[i].size() ; objInd++)
        {
            m = QMatrix4x4();
            m.setToIdentity();
            if (i == FLAGS)
            {
                m.translate(sokoban.get(i).at(objInd).x(), -0.49, sokoban.get(i).at(objInd).y());
                m.rotate(-90, 1, 0, 0);
            }
            else if (i == SURFACE)
            {
                m.translate(sokoban.xSize / 2.0, -0.5, sokoban.ySize / 2.0);
                m.rotate(-90, 1, 0, 0);
                m.scale(sokoban.xSize + 1, sokoban.ySize + 1, 1);
            }
            else if (i == CHARACTER)
            {
                m.translate(sokoban.get(i).at(objInd).x(), -0.5, sokoban.get(i).at(objInd).y());
                m.rotate(sokoban.orientation, 0, 1, 0);
            }
            else
            {
                m.translate(sokoban.get(i).at(objInd).x(), 0, sokoban.get(i).at(objInd).y());
            }

            objectMatrixes[i].replace(objInd, m);
        }
    }
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
    //PHONG

    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                 ":/shaders/vertshader_phong.glsl");
    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                 ":/shaders/fragshader_phong.glsl");
    for (GLuint i = 0 ; i < COUNTSHADER ; i++)
    {
        shaderProgram[i].link();

        modelShaderTransform[i]= shaderProgram[i].uniformLocation("modelTransform");
        projLocation[i] = shaderProgram[i].uniformLocation("projTransform");
        viewLocation[i] = shaderProgram[i].uniformLocation("viewTransform");
        normalLocation[i] = shaderProgram[i].uniformLocation("normalTransform");
        samplerLocation[i] = shaderProgram[i].uniformLocation("samplerUniform");
        lightColorLocation[i] = shaderProgram[i].uniformLocation("lightColor");
        lightPositionLocation[i] = shaderProgram[i].uniformLocation("lightPosition");
        materialLocation[i] = shaderProgram[i].uniformLocation("material");
        placedBoxLocation[i] = shaderProgram[i].uniformLocation("placedBox");
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

    if (sokoban.finished)
    {
        timer.start(FPS);
        animationIsRunning = true;
        animate();
    }

    updateObjects();
    updateProjectionMatrix();

    float lightColor[3] = {1.0, 1.0, 1.0};
    float material[4] = {0.2, 0.8, 0.0, 1.0};
    float lightPosition[3] = {100.0, 100.0, 150.0};

    shaderProgram[currentShade].bind();

    glUniformMatrix4fv(projLocation[currentShade], 1, GL_FALSE, (GLfloat *) projMatrix.data());
    glUniformMatrix4fv(viewLocation[currentShade], 1, GL_FALSE, (GLfloat *) viewMatrix.data());
    glUniform3fv(lightColorLocation[currentShade], 1, lightColor);
    glUniform3fv(lightPositionLocation[currentShade], 1, lightPosition);
    glUniform4fv(materialLocation[currentShade], 1, material);
    glUniform1i(samplerLocation[currentShade], 0);

    //models
    bool placed;

    for (int modelType = 0 ; modelType < MODELINDEX::COUNT ; modelType++)
    {
        placed = false;
        int i = 0;
        for (QMatrix4x4 matrix : objectMatrixes[modelType])
        {
            if(modelType == BOXES)
            {
                placed = sokoban.boxPlaced(i);
            }
            glUniform1i(placedBoxLocation[currentShade], placed);

            glUniformMatrix3fv(normalLocation[currentShade], 1, GL_FALSE, (GLfloat *) matrix.normalMatrix().data());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture[modelType]);

            glBindVertexArray(vao[modelType]);
            glUniformMatrix4fv(modelShaderTransform[currentShade], 1, GL_FALSE, (GLfloat *) matrix.data());
            glDrawArrays(GL_TRIANGLES, 0, modelSize[modelType]);
            i++;
        }
    }

    shaderProgram[currentShade].release();
}

/*
void MainView::freeFallJump(MODELINDEX jumper, MODELINDEX surface, qreal initialVelocity)
{
    const qreal G = -9.8;

    if (yTranslation[jumper] < yTranslation[surface])
        ti[jumper] = 0;
    ti[jumper] += FPS/1000; //since there's a frame every 1000/60 milisseconds, we should add 1/60 of a second to the time variable at each frame

    yTranslation[jumper] = yTranslation[surface] + initialVelocity * ti[jumper] + 0.5 * G * ti[jumper] * ti[jumper];
}
*/

void MainView::animate() {
    if (animationIsRunning)
    {
        sokoban.orientation += 10;
    }
}

/*
void MainView::AddRotation(int index, qreal x, qreal y, qreal z)
{
    xRotation[index] += x;
    xRotation[index] = fmod(xRotation[index] , 360.0);
    yRotation[index] += y;
    yRotation[index] = fmod(yRotation[index] , 360.0);
    zRotation[index] += z;
    zRotation[index] = fmod(zRotation[index] , 360.0);
}
*/

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

/*
void MainView::setRotation(qreal rotateX, qreal rotateY, qreal rotateZ)
{
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";

    //qreal x = ((qreal) rotateX) / 360;
    xGeneralRotation = rotateX;
    yGeneralRotation = rotateY;
    zGeneralRotation = rotateZ;

    update();
}
*/

/*
void MainView::setScale(int scale)
{
    qDebug() << "Scale changed to " << scale;

    qreal s = (qreal) scale;

    s = s / 100.0;
    generalScale = s;

    update();
}
*/

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
