#include "triangle.h"

#include <cmath>

using namespace std;

Hit Triangle::intersect(Ray const &ray) {
    return Hit::NO_HIT();
}


Triangle::Triangle(Point const &pA, Point const &pB, Point const &pC)
:
    pointA(pA),
    pointB(pB),
    pointC(pC)
{}
