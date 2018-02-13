#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vertex.h"
struct Triangle{
    Vertex v[3]; //three vertices that compose the triangle

    Triangle(Vertex vc[3]) {
        v[0] = vc[0];
        v[1] = vc[1];
        v[2] = vc[2];
    }

    Triangle(){
    }

    Vertex * toVArray(){
        return v;
    }
};

#endif // TRIANGLE_H
