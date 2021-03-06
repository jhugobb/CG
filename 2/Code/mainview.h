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

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram shaderProgram;

public:
    enum MODELINDEX
    {
        CUBE = 0,
        PYRAMID,
        MODEL,
        COUNT
    };

    GLuint vbo[MODELINDEX::COUNT]; //[cube, pyramid, model]
    GLuint vao[MODELINDEX::COUNT]; //[cube, pyramid, model]
    QMatrix4x4 cubeMatrix;
    QMatrix4x4 pyramidMatrix;
    QMatrix4x4 modelMatrix;
    qreal modelScale = 1.0;
    QMatrix4x4 projMatrix = QMatrix4x4();
    qreal xRotation = 0;
    qreal yRotation = 0;
    qreal zRotation = 0;
    qreal scale = 1;
    GLint modelShaderTransform;
    GLint projLocation;
    int modelSize;


    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    void rotate(qreal x, qreal y, qreal z);
    void turnCubeToOriginal();
    void turnPyramidToOriginal ();
    void turnModelToOriginal();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);

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

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();

};

#endif // MAINVIEW_H
