#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere : public Object
{
  public:
    Sphere(Point const &pos, double radius);

    virtual Hit intersect(Ray const &ray);

    virtual Point2D textureCoordinates(Point const &p);

    Point const position;
    double const r;
};

#endif
