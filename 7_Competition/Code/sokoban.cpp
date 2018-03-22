#include "sokoban.h"
#include "mainview.h"
#include "math.h"

Sokoban::Sokoban()
{
    walls = QVector<QPoint>();
    boxes = QVector<QPoint>();
    flags = QVector<QPoint>();
    character = QPoint();
    orientation = 0;
    xSize = 0;
    ySize = 0;
    changedBox = -1;
}

Sokoban::Sokoban(QString dir)
{
    walls = QVector<QPoint>();
    boxes = QVector<QPoint>();
    flags = QVector<QPoint>();
    character = QPoint();
    orientation = 0;
    xSize = 0;
    ySize = 0;
    changedBox = -1;

    QFile file(dir);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw QException();

    QTextStream in(&file);
    int x, y;
    y = 0;
    for (QString line = in.readLine(); !line.isNull() ; line = in.readLine())
    {
        for (x = 0 ; x < line.size() ; x++)
        {
            switch (line.at(x).unicode()) {
            case '#':
                walls.append(QPoint(x, y));
                break;
            case 'H':
                boxes.append(QPoint(x, y));
                break;
            case '.':
                flags.append(QPoint(x, y));
                break;
            case 'o':
                character = QPoint(x, y);
                break;
            default:
                break;
            }
        }
        if (xSize < x - 1)
            xSize = x - 1;

        y++;
    }
    if (ySize < y - 1)
        ySize = y - 1;

    assert(!character.isNull());
    assert(!walls.isEmpty());
    assert(boxes.size() == flags.size());

    checkIfIsFinished();
}

int Sokoban::size()
{
    return 1 + walls.size() + boxes.size() + flags.size();
}

QVector<QPoint> Sokoban::get(int type)
{
    if (type == MainView::MODELINDEX::BOXES)
    {
        return boxes;
    }
    if (type == MainView::MODELINDEX::CHARACTER)
    {
        QVector<QPoint> r = QVector<QPoint>();
        r.append(character);
        return r;
    }
    if (type == MainView::MODELINDEX::WALLS)
    {
        return walls;
    }
    if (type == MainView::MODELINDEX::FLAGS)
    {
        return flags;
    }

    return QVector<QPoint>();
}

void Sokoban::rotateCharacter (bool turnRight)
{
    qreal tmp = orientation;
    changedBox = -1;
    if (!finished)
    {
        if (turnRight)
        {
            tmp -= 90;
        }
        else
        {
            tmp += 90;
        }
        orientation = fmod(tmp, 360);
    }
}

void Sokoban::makeMovement(QPoint move)
{
    QPoint tmp = character + move;

    if (!finished && (!(walls.indexOf(tmp) != -1 || (boxes.indexOf(tmp) != -1 && (walls.indexOf(tmp + move) != -1 || boxes.indexOf(tmp + move) != -1))))) //first, checks if there is no wall, then checks if there is no box followed by a wall or another box
    {
        character = tmp;
        int i = boxes.indexOf(character); //checks if
        if (i != -1)                      //there's a box to be moved
        {
            boxes.replace(i,character + move);
            changedBox = i;
        }
    }
}

void Sokoban::moveCharacter (bool toFront)
{
    changedBox = -1;
    if (orientation == 0)
    {
        if (toFront)
            makeMovement(QPoint(1, 0));
        else
            makeMovement(QPoint(-1, 0));
    }
    else if (orientation == 90 || orientation == -270)
    {
        if (!toFront)
            makeMovement(QPoint(0, 1));
        else
            makeMovement(QPoint(0, -1));
    }
    else if (orientation == -90 || orientation == 270)
    {
        if (toFront)
            makeMovement(QPoint(0, 1));
        else
            makeMovement(QPoint(0, -1));
    }
    else if (abs(orientation) == 180)
    {
        if (!toFront)
            makeMovement(QPoint(1, 0));
        else
            makeMovement(QPoint(-1, 0));
    }

    checkIfIsFinished();
}

bool Sokoban::boxPlaced(int index)
{
    return (flags.contains(boxes.at(index)));
}

void Sokoban::checkIfIsFinished()
{
    bool tmp = true;

    for (int i = 0 ; i < boxes.size() && tmp ; i++)
    {

        qDebug() << "IT HAPPENS2";
        if (!flags.contains(boxes.at(i)))
        {
            qDebug() << "IT HAPPENS1";
            tmp = false;
        }
    }
    finished = tmp;

}
