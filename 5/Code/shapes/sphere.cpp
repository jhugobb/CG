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

Point2D Sphere::textureCoords(Point const &p) {
    Vector n = (p - position).normalized(); //try not normalized
    Triple rN = rotation.normalized();

    // Rotation Matrix
    double rotMatrix[3][3];
    rotMatrix[0][0] = cos(angle) + (rN.x*rN.x)*(1-cos(angle));
    rotMatrix[0][1] = rN.x*rN.y*(1-cos(angle))-rN.z*sin(angle);
    rotMatrix[0][2] = rN.x*rN.z*(1-cos(angle)) + rN.y*sin(angle);
    rotMatrix[1][0] = rN.y * rN.x * (1 - cos(angle)) + rN.z * sin(angle);
    rotMatrix[1][1] = cos(angle) + (rN.y * rN.y) * (1 - cos(angle));
    rotMatrix[1][2] = rN.y * rN.z * (1 - cos(angle)) - rN.x * sin(angle);
    rotMatrix[2][0] = rN.x * rN.z * (1 - cos(angle)) - rN.y * sin(angle);
    rotMatrix[2][1] = rN.y * rN.z * (1 - cos(angle)) + rN.x * sin(angle);
    rotMatrix[2][2] = cos(angle) + (rN.z * rN.z) * (1 - cos(angle));

    Triple r;
    r.x = n.x * rotMatrix[0][0] + n.y * rotMatrix[0][1] + n.z * rotMatrix[0][2];
    r.y = n.x * rotMatrix[1][0] + n.y * rotMatrix[1][1] + n.z * rotMatrix[1][2];
    r.z = n.x * rotMatrix[2][0] + n.y * rotMatrix[2][1] + n.z * rotMatrix[2][2];

    return Point2D(0.5 + (atan2(r.y, r.x) / (2 * M_PI)), 1 - (acos(r.z) / M_PI));
}

Sphere::Sphere(Point const &pos, double radius, Triple rot, double a)
:
    position(pos),
    r(radius),
    rotation(rot),
    angle(a)
{}
