#ifndef SQUARE_H
#define SQUARE_H
#include "vertex.h"
#include "triangle.h"

struct Square {
    Triangle t[2];

    Square (Triangle t1, Triangle t2) {

        t[0] = t1;
        t[1] = t2;
    }

    Square (Vertex v[4]) { //receives 4 vertexes
        Vertex tmp1[3] = {v[0], v[1], v[2]};
        Vertex tmp2[3] = {v[2], v[3], v[0]};

        t[0] = Triangle(tmp1);
        t[1] = Triangle(tmp2);

    }

    Square (){
    }

    void toVArray(Vertex v[6]){
        Vertex *vt1, *vt2;
        vt1 = t[0].toVArray();
        vt2 = t[1].toVArray();

        int i;
        for(i = 0 ; i < 3 ; i++) {
            v[i] = vt1[i];
        }
        for(i = 0 ; i < 3 ; i++) {
            v[i + 3] = vt2[i];
        }
    }
};

#endif // SQUARE_H
