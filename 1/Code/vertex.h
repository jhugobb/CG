#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
    float coord[2]; // [x,y]
    float color[3]; // [r, g, b]

    Vertex(float givenx, float giveny, float givenr, float giveng, float givenb) {
        coord[0] = givenx;
        coord[1] = giveny;
        color[0] = givenr;
        color[1] = giveng;
        color[2] = givenb;
    }

    Vertex() {

    }

};
#endif // VERTEX_H
