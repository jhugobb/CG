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

#define EYEDISTANCE 10
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation


public:
    enum MODELINDEX : int
    {
        JUPITER = 0,
        MOON1,
        MOON2,
        MOON3,
        COUNT
    };

    GLuint vbo[MODELINDEX::COUNT]; // [model]
    GLuint vao[MODELINDEX::COUNT]; // [model]
    GLuint texture[MODELINDEX::COUNT]; // [model]

    QMatrix4x4 objectMatrix[MODELINDEX::COUNT];
    QMatrix4x4 projMatrix = QMatrix4x4();
    qreal perspectiveRotation = 0;
    qreal perspectiveHeight = 90; //also in degrees
    qreal projectionRatio = 1;

    qreal xRotation[MODELINDEX::COUNT];
    qreal yRotation[MODELINDEX::COUNT];
    qreal zRotation[MODELINDEX::COUNT];
    qreal xTranslation[MODELINDEX::COUNT];
    qreal yTranslation[MODELINDEX::COUNT];
    qreal zTranslation[MODELINDEX::COUNT];
    qreal objectScale[MODELINDEX::COUNT];
    qreal generalScale = 1;
    qreal xGeneralRotation = 0;
    qreal yGeneralRotation = 0;
    qreal zGeneralRotation = 0;
    int modelSize[MODELINDEX::COUNT];
    float ti = 0;
    float distanceToJupiter[MODELINDEX::COUNT];

    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD, COUNTSHADER
    };

    QOpenGLShaderProgram shaderProgram[COUNTSHADER];
    GLint modelShaderTransform[COUNTSHADER];
    GLint projLocation[COUNTSHADER];
    GLint normalLocation[COUNTSHADER];
    GLint samplerLocation[COUNTSHADER];
    GLint lightPositionLocation[COUNTSHADER];
    GLint materialColorLocation[COUNTSHADER];
    GLint lightColorLocation[COUNTSHADER];
    GLint materialLocation[COUNTSHADER];
    GLuint currentShade = 0;

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
