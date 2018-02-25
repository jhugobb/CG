#include "triangle.h"

#include <cmath>

using namespace std;

Hit Triangle::intersect(Ray const &ray) {
    //MT algorithm
    double u, v;
    Vector AB = pointB - pointA;
    Vector AC = pointC - pointA;
    Vector pV = ray.D.cross(AC);
    double det = AB.dot(pV);
    
    if (det < 0) {
        return Hit::NO_HIT();
    }

    double invDet = 1 / det;
    Vector tV = ray.O - pointA;

    u = tV.dot(pV) * invDet;
    if (u < 0 || u > 1) {
        return Hit::NO_HIT();
    }

    Vector qV = tV.cross(AB);
    v = ray.D.dot(qV) * invDet;

    if (v < 0 || u + v > 1) {
        return Hit::NO_HIT();
    }

    double t = AC.dot(qV) * invDet;

    return Hit(t, AB.cross(AC));
}


Triangle::Triangle(Point const &pA, Point const &pB, Point const &pC)
:
    pointA(pA),
    pointB(pB),
    pointC(pC)
{}
