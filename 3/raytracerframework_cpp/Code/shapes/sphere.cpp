#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    // place holder for actual intersection calculation
    Vector OC = (position - ray.O);
    double tca = OC.dot(ray.D);
    if (tca < 0) {
        return Hit::NO_HIT();
    }
    double d2 = OC.dot(OC) - tca * tca; //Why doesn't this work with "1 - tca^2"
    if(d2 < 0) {
        return Hit::NO_HIT();
    }
    double thc = sqrt(r*r - d2);
    double t0 = tca - thc;
    double t1 = tca + thc;

    if (t0 > t1) {
        swap(t0, t1);
    }
    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) {
            return Hit::NO_HIT();
        }
    }
    double t = t0;
    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/
    Point p = ray.O + t * ray.D;

    Vector N = (p - position).normalized();

    return Hit(t,N);
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
