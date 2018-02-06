#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
    float x;
    float y;
    float r;
    float g;
    float b;

    Vertex(float givenx, float giveny, float givenr, float giveng, float givenb) {
        x = givenx;
        y = giveny;
        r = givenr;
        g = giveng;
        b = givenb;
    }
};
#endif // VERTEX_H
