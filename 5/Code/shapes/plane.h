#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane : public Object
{
  public:
    Plane(Point const &p0, Triple const &N);

    virtual Hit intersect(Ray const &ray);
    virtual Point2D textureCoordinates(Point const &p);
    /* YOUR DATA MEMBERS HERE*/
    Point const p0;
    Triple const N;
};

#endif
