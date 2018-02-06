#ifndef VERTEX_H
#define VERTEX_H

typedef struct vertex
{
    float x;
    float y;
    float r;
    float g;
    float b;
    vertex(float x, float y, float r, float g, float b) {
        this->x = x;
        this->y = y;
        this->r = r;
        this->g = g;
        this->b = b;
    }
}* Vertex;

#endif // VERTEX_H
