#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane : public Object
{
  public:
    Plane(Point const &p0, Triple const &N);

    virtual Hit intersect(Ray const &ray);

    /* YOUR DATA MEMBERS HERE*/
    Point const p0;
    Triple const N;
};

#endif
