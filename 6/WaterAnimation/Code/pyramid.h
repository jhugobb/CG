#ifndef PYRAMID_H
#define PYRAMID_H

struct Pyramid {
    //The pyramid is composed out of one square and 4 triangles.
    Square sq;
    Triangle tr[4];

    /**
     * @brief Pyramid construtor
     * @param v The first is the main vertex and the following compose the base.
     */
    Pyramid (Vertex v[5]) {
        Vertex t1[3] = {v[0], v[1], v[2]};
        Vertex t2[3] = {v[0], v[2], v[3]};
        Vertex t3[3] = {v[0], v[3], v[4]};
        Vertex t4[3] = {v[0], v[4], v[1]};
        tr[0] = Triangle(t1);
        tr[1] = Triangle(t2);
        tr[2] = Triangle(t3);
        tr[3] = Triangle(t4);

        Vertex baseV[4] = {v[4], v[3], v[2], v[1]};
        sq = Square(baseV);
    }

    void toVArray(Vertex v[18]){
        //copies the triangles
        for (int i = 0 ; i < 4 ; i++) {
            tr[i].toVArray(v + (i * 3));
        }
        //copies the base
        sq.toVArray(v + 12);
    }
};

#endif // PYRAMID_H
