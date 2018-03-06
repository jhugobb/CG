#include "plane.h"

#include <cmath>

constexpr float kEpsilon = 1e-6;

Hit Plane::intersect(Ray const &ray)
{
    /* Your intersect calculation goes here */
    double denom = N.dot(ray.D);
    if (fabs(denom) >= kEpsilon) {
        Triple p0O = p0 - ray.O;
        double t = p0O.dot(N) / denom;
        if (t >= 0)
            return Hit(t, N);
    }
    return Hit::NO_HIT();
}

Plane::Plane(Point const &p0, Triple const &N)
:
    p0(p0),
    N(N.normalized())
{}
