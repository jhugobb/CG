#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere: public Object
{
    public:
      Sphere(Point const &pos, double radius, Triple rot, double angle);

      virtual Hit intersect(Ray const &ray);

      virtual Point2D textureCoords(Point const &p);

      Point const position;
      double const r; // [radius, axis]
      Triple const rotation;
      double const angle;
};

#endif
