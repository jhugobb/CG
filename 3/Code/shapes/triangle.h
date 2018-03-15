#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"
#include "../vertex.h"

class Triangle : public Object
{
  public:
    Triangle(Point const &pos1, Point const &pos2, Point const &pos3);

    virtual Hit intersect(Ray const &ray);

    Point const v1;
    Point const v2;
    Point const v3;
    Triple const N;
};

#endif
