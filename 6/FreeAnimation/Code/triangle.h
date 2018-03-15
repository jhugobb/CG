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

    void toVArray(Vertex ve[3]){
        for(int i = 0 ; i < 3 ; i++)
            ve[i] = v[i];
    }
};

#endif // TRIANGLE_H
