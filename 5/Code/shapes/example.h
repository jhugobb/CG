#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "../object.h"

class Example : public Object
{
  public:
    Example(/* YOUR DATA MEMBERS HERE*/);

    virtual Hit intersect(Ray const &ray);
    virtual Point2D textureCoordinates(Point const &p);
    /* YOUR DATA MEMBERS HERE*/
};

#endif
