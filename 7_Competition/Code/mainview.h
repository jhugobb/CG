#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"
#include "vertex.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <memory>
#include <QMatrix4x4>
#include "sokoban.h"

#define FPS 1000.0/60.0
#define NROFLVLS 10
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation
    Sokoban sokoban;
    int currentLevel = 0;


public:
    enum MODELINDEX : int
    {
        CHARACTER = 0,
        WALLS,
        BOXES,
        FLAGS,
        SURFACE,
        COUNT
    };

    enum PERSONMODE
    {
        FIRST,
        THIRD
    };

    enum TEXTUREMODE
    {
        MINECRAFT,
        STARWARS
    };

    GLuint vbo[MODELINDEX::COUNT]; // [model]
    GLuint vao[MODELINDEX::COUNT]; // [model]
    GLuint texture[MODELINDEX::COUNT]; // [model]

    QVector<QMatrix4x4> objectMatrixes[COUNT];
    QMatrix4x4 projMatrix = QMatrix4x4();
    QMatrix4x4 viewMatrix = QMatrix4x4();

    bool animationIsRunning = false;
    qreal perspectiveRotation = 0;
    qreal perspectiveHeight = 45; //also in degrees
    qreal perspectiveDistance = 15;
    qreal projectionRatio = 1;
    PERSONMODE personMode = THIRD;
    //Mouse perspectio rotation variables
    QPoint scrollOrigin = QPoint(-1, -1);
    qreal initialPerspectiveRotation;
    qreal initialPerspectiveHeight;

    int modelSize[MODELINDEX::COUNT];
    float ti[MODELINDEX::COUNT];

    enum ShadingMode : GLuint
    {
        PHONG = 0, COUNTSHADER
    };

    QOpenGLShaderProgram shaderProgram[COUNTSHADER];
    GLint modelShaderTransform[COUNTSHADER];
    GLint projLocation[COUNTSHADER];
    GLint viewLocation[COUNTSHADER];
    GLint normalLocation[COUNTSHADER];
    GLint samplerLocation[COUNTSHADER];
    GLint lightPositionLocation[COUNTSHADER];
    GLint lightColorLocation[COUNTSHADER];
    GLint materialLocation[COUNTSHADER];
    GLint placedBoxLocation[COUNTSHADER];
    ShadingMode currentShade = PHONG;
    TEXTUREMODE textureMode = MINECRAFT;

    MainView(QWidget *parent = 0);
    ~MainView();

    void rotate(qreal x, qreal y, qreal z);

    // Functions for widget input events
    void setRotation(qreal rotateX, qreal rotateY, qreal rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);
    void loadTexture(QString file, GLuint texturePtr);
    void animate();
    void AddRotation(int index, qreal x, qreal y, qreal z);
    void updateObjects();
    void loadModel(MODELINDEX modelNr,  char const *objPath, char const *texturePath);
    void initializeObjectsAttributes();
    void updateProjectionMatrix();
    void freeFallJump(MODELINDEX jumper, MODELINDEX surface, qreal initialVelocity);
    void loadSokoban();

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
    QVector<quint8> imageToBytes(QImage image);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();

};

#endif // MAINVIEW_H
