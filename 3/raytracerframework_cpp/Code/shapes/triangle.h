#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"

class Triangle : public Object
{
  public:
    Triangle(Point const &pA, Point const &pB, Point const &pC);

    virtual Hit intersect(Ray const &ray);

    Point const pointA;
    Point const pointB;
    Point const pointC;
};

#endif
