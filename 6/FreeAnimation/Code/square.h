#ifndef SQUARE_H
#define SQUARE_H
#include "vertex.h"
#include "triangle.h"

struct Square {
    //The square is composed out of 2 triangles
    Triangle t[2];

    Square (Triangle t1, Triangle t2) {

        t[0] = t1;
        t[1] = t2;
    }

    /**
     * @brief Square
     * @param v Receives 4 vertexes that must be in clock-wise order
     */
    Square (Vertex v[4]) {
        Vertex tmp1[3] = {v[0], v[1], v[2]};
        Vertex tmp2[3] = {v[2], v[3], v[0]};

        t[0] = Triangle(tmp1);
        t[1] = Triangle(tmp2);

    }

    Square (){
    }

    void toVArray(Vertex v[6]){
        t[0].toVArray(v);
        t[1].toVArray(v + 3);
    }
};

#endif // SQUARE_H
