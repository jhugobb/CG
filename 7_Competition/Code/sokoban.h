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

    Sokoban();
    Sokoban(QString file);
    QVector<QPoint> get(int type);

    int size();
    void rotateCharacter (bool turnRight);
    void moveCharacter (bool toFront);
private:
    void makeMovement (QPoint move);
};
#endif // SOKOBAN_H
