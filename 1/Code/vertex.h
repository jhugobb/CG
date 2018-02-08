#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
    float coor[2];
    float color[3];

    Vertex(float x, float y, float r, float g, float b) {
        this->coor[0] = x;
        this->coor[1] = y;
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    }

    Vertex(){
    }
};

#endif // VERTEX_H
