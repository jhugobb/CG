#ifndef SOKOBAN_H
#define SOKOBAN_H

#include <QPoint>
#include <QVector>
#include <QFile>
#include <QException>
#include <QTextStream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <QDebug>


class Sokoban
{
public:

    QVector<QPoint> walls;
    QVector<QPoint> boxes;
    QPoint character;
    QVector<QPoint> flags;
    int orientation;
    bool finished = false;
    int xSize;
    int ySize;
    int changedBox;

    Sokoban();
    Sokoban(QString file);
    QVector<QPoint> get(int type);

    int size();
    void rotateCharacter (bool turnRight);
    void moveCharacter (bool toFront);
    bool boxPlaced(int index);
private:
    void makeMovement (QPoint move);
    void checkIfIsFinished ();
};
#endif // SOKOBAN_H
