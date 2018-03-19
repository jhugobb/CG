#include "mainview.h"
#include "math.h"

#include <QDebug>

#define SCROLLSPEED 0.5
#define ZOOMSPEED 1


// Triggered by pressing a key
void MainView::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
    case 'D':
        sokoban.rotateCharacter(true);
        qDebug() << "D pressed, value: " << sokoban.orientation;
        break;
    case 'A':
        sokoban.rotateCharacter(false);
        qDebug() << "A pressed, value: " << sokoban.orientation;
        break;
    case 'S':
        qDebug() << "Orientation: " << sokoban.orientation;
        sokoban.moveCharacter(false);
        qDebug() << "S pressed, value ";
        break;
    case 'W':
        qDebug() << "Orientation: " << sokoban.orientation;
        sokoban.moveCharacter(true);
        qDebug() << "W pressed, value: ";
        break;
    /*case ' ':
        if (animationIsRunning)
        {
            timer.stop();
            animationIsRunning = false;
        }
        else
        {
            timer.start(FPS);
            animationIsRunning = true;
        }
        qDebug() << "SPACE pressed, animation is running?: " << animationIsRunning;
        break;*/
    default:
        // ev->key() is an integer. For alpha numeric characters keys it equivalent with the char value ('A' == 65, '1' == 49)
        // Alternatively, you could use Qt Key enums, see http://doc.qt.io/qt-5/qt.html#Key-enum
        qDebug() << ev->key() << "pressed";
        break;
    }

    // Used to update the screen after changes
    update();
}

// Triggered by releasing a key
void MainView::keyReleaseEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
    case 'A':
        qDebug() << "A released";
        break;
    default:
        qDebug() << ev->key() << "released";
        break;
    }

    update();
}

// Triggered by clicking two subsequent times on any mouse button
// It also fires two mousePress and mouseRelease events!
void MainView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse double clicked:" << ev->button();

    update();
}

// Triggered when moving the mouse inside the window (only when the mouse is clicked!)
void MainView::mouseMoveEvent(QMouseEvent *ev)
{
    qDebug() << "x" << ev->x() << "y" << ev->y();
    qreal tmpRotation, tmpHeight;
    if (scrollOrigin != QPoint(-1, -1))
    {
        tmpRotation = initialPerspectiveRotation + (scrollOrigin.x() - ev->x()) * SCROLLSPEED;
        perspectiveRotation = fmod(tmpRotation, 360);

        tmpHeight = initialPerspectiveHeight + (scrollOrigin.y() - ev->y()) * SCROLLSPEED;
        if (tmpHeight >= 1 && tmpHeight <= 90)
            perspectiveHeight = tmpHeight;
    }

    qDebug() <<perspectiveHeight;

    update();
}

// Triggered when pressing any mouse button
void MainView::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button pressed:" << ev->button();
    scrollOrigin = QPoint(ev->x(), ev->y());
    initialPerspectiveRotation = perspectiveRotation;
    initialPerspectiveHeight = perspectiveHeight;

    update();
    // Do not remove the line below, clicking must focus on this widget!
    this->setFocus();
}

// Triggered when releasing any mouse button
void MainView::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button released" << ev->button();
    scrollOrigin = QPoint(-1, -1);
    update();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainView::wheelEvent(QWheelEvent *ev)
{
    qreal tmp = perspectiveDistance;
    if (ev->delta() > 0)
        tmp = perspectiveDistance + ZOOMSPEED;
    else if (ev->delta() < 0)
        tmp = perspectiveDistance - ZOOMSPEED;
    if (tmp >= 2 && tmp < 60)
        perspectiveDistance = tmp;

    qDebug() << "Mouse wheel:" << ev->delta();

    update();
}
