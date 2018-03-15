#ifndef CUBE_H
#define CUBE_H
#include "square.h"

struct Cube {

    //The cube is composed out of 6 squares
    Square sq[6];

    /**
     * @brief Cube
     * @param v 8 Vertexes of the cube. Firstly are assigned the 4 vertexes of the frontal face, then the 4 from the back face. Both have to be defined in counter clock wise order (from an outside perspective).
     */
    Cube (Vertex v[8]) {
        Vertex tmp[4];
        tmp[0] = v[0]; //frontal square
        tmp[1] = v[1];
        tmp[2] = v[2];
        tmp[3] = v[3];
        sq[0] = Square(tmp);

        tmp[0] = v[3]; //up square
        tmp[1] = v[2];
        tmp[2] = v[6];
        tmp[3] = v[5];
        sq[1] = Square(tmp);

        tmp[0] = v[0]; //left square
        tmp[1] = v[3];
        tmp[2] = v[5];
        tmp[3] = v[4];
        sq[2] = Square(tmp);

        tmp[0] = v[0]; //down square
        tmp[1] = v[4];
        tmp[2] = v[7];
        tmp[3] = v[1];
        sq[3] = Square(tmp);

        tmp[0] = v[1]; //right square
        tmp[1] = v[7];
        tmp[2] = v[6];
        tmp[3] = v[2];
        sq[4] = Square(tmp);

        tmp[0] = v[4]; //Back square
        tmp[1] = v[5]; //perhaps swap the order (counter clock wise)
        tmp[2] = v[6];
        tmp[3] = v[7];
        sq[5] = Square(tmp);
    }

    void toVArray(Vertex v[36]) {
        int i;
        Vertex tmp[6];
        for (int s = 0 ; s < 6 ; s++) {
            sq[s].toVArray(tmp);
            for (i = 0 ; i < 6 ; i++) {
                v[s * 6 + i] = tmp[i];
            }
        }
    }
};

#endif // CUBE_H
