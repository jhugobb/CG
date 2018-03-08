#include "sphere.h"
#include "../solveQ.h"

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

    // Calculation of the intersections with the sphere with an analytic method
    Triple OC = ray.O - position;
    float t0, t1;
    float a = ray.D.dot(ray.D);
    float b = 2 * ray.D.dot(OC);
    float c = OC.dot(OC) - (r * r);
    if (!solveQuadratic(a, b, c, t0, t1))
        return Hit::NO_HIT();
    if (t0 < 0)
    {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0)
            return Hit::NO_HIT(); // both t0 and t1 are negative
    }
    // Let's use the closest distance to know wich sphere is closer to the camera
    double t = t0;

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/
    Vector P = ray.O + ray.D * t;
    Vector N = (P - position).normalized();

    return Hit(t, N);
}

Point2D Sphere::textureCoordinates(Point const &p)
{
    Vector n = (position - p).normalized(); //try not normalized
    //std::cout << "\nVALLEEEEE" << acos(n.z / abs(n.x)) << "\n";
    return Point2D(0.5 + (atan2(n.z, n.x) / (2 * M_PI)) , 0.5 - (asin(n.y) / M_PI));
}

Sphere::Sphere(Point const &pos, double radius)
    : position(pos),
      r(radius)
{
}
