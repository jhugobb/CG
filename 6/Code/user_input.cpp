#include "mainview.h"
#include "math.h"

#include <QDebug>

#define SCROLLSPEED 3

// Triggered by pressing a key
void MainView::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
    case 'D':
        if (perspectiveRotation < 360 - SCROLLSPEED)
        {
            perspectiveRotation += SCROLLSPEED;
        }
        else
        {
            perspectiveRotation = 0;
        }
        qDebug() << "D pressed, value: " << perspectiveRotation;
        break;
    case 'A':
        if (perspectiveRotation > SCROLLSPEED)
        {
            perspectiveRotation -= SCROLLSPEED;
        }
        else
        {
            perspectiveRotation = 359;
        }
        qDebug() << "A pressed, value: " << perspectiveRotation;
        break;
    case 'S':
        if(perspectiveHeight < 180 - SCROLLSPEED)
            perspectiveHeight += SCROLLSPEED;
        qDebug() << "S pressed, value: " << perspectiveHeight;
        break;
    case 'W':
        if(perspectiveHeight > SCROLLSPEED)
            perspectiveHeight -= SCROLLSPEED;
        qDebug() << "W pressed, value: " << perspectiveHeight;
        break;
    case ' ':
        perspectiveRotation = 0;
        perspectiveHeight = 90;
        qDebug() << "SPACE pressed, value: " << perspectiveHeight;
        break;
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

    update();
}

// Triggered when pressing any mouse button
void MainView::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button pressed:" << ev->button();

    update();
    // Do not remove the line below, clicking must focus on this widget!
    this->setFocus();
}

// Triggered when releasing any mouse button
void MainView::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button released" << ev->button();

    update();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainView::wheelEvent(QWheelEvent *ev)
{
    // Implement something
    qDebug() << "Mouse wheel:" << ev->delta();

    update();
}
