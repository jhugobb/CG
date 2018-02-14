#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
    float coord[3]; // [x,y,z]
    float color[3]; // [r,g,b]

    Vertex(float givenx, float giveny, float givenz, float givenr, float giveng, float givenb) {
        coord[0] = givenx;
        coord[1] = giveny;
        coord[2] = givenz;
        color[0] = givenr;
        color[1] = giveng;
        color[2] = givenb;
    }

    Vertex() {

    }

};
#endif // VERTEX_H
