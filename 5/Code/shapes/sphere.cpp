#include "sphere.h"
#include "../solveQ.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    // Calculation of the intersections with the sphere with an analytic method
    Triple OC = ray.O - position;
    float t0, t1;
    float a = ray.D.dot(ray.D);
    float b = 2 * ray.D.dot(OC);
    float c = OC.dot(OC) - (r*r);
    if (!solveQuadratic(a, b, c, t0, t1))
        return Hit::NO_HIT();
    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0)
            return Hit::NO_HIT(); // both t0 and t1 are negative
    }
    // Let's use the closest distance to know wich sphere is closer to the camera
    double t = t0;

    Vector P = ray.O + ray.D * t;
    Vector N = (P - position).normalized() ;

    return Hit(t,N);
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
