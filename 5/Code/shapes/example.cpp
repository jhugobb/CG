#include "example.h"

#include <cmath>

Hit Example::intersect(Ray const &ray)
{
    /* Your intersect calculation goes here */

    double t = 0 /* = ... */;
    Vector N /* = ... */;

    return Hit(t, N);
}
Point2D Example::textureCoordinates(Point const &p){
    return Point2D(0,0);
}

Example::Example(/* YOUR DATAMEMBERS HERE */)
//:
// See sphere.cpp how to initialize your data members
{}
