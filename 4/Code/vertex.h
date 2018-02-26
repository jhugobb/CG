#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
    float coord[3]; // [x,y,z]
    float normal[3]; // [x, y, z]

    Vertex(float givenx, float giveny, float givenz, float givenxn, float givenyn, float givenzn) {
        coord[0] = givenx;
        coord[1] = giveny;
        coord[2] = givenz;
        normal[0] = givenxn;
        normal[1] = givenyn;
        normal[2] = givenzn;
    }

    Vertex() {

    }

};
#endif // VERTEX_H
