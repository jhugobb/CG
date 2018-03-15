#include "triangle.h"
#include "../solveQ.h"
#include "../vertex.h"

#include <cmath>

using namespace std;

constexpr float kEpsilon = 1e-8;

Hit Triangle::intersect(Ray const &ray)
{
    // Calculation of the intersections with the triangle with inside-out method
    //Find vectors for two edges sharing vertex/point p1
    Triple edge1 = v2 - v1;
    Triple edge2 = v3 - v1;
    Triple h = ray.D.cross(edge2);
    float a = edge1.dot(h);

    if (a > -kEpsilon && a < kEpsilon)
        return Hit::NO_HIT();

    float f = 1 / a;
    Triple s = ray.O - v1;
    float u = f * (s.dot(h));

    if (u < 0.0 || u > 1.0)
        return Hit::NO_HIT();

    Triple q = s.cross(edge1);
    float v = f * ray.D.dot(q);

    if (v < 0.0 || u + v > 1.0)
        return Hit::NO_HIT();

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * edge2.dot(q);
    if (t > kEpsilon) // ray intersection
    {   
        return Hit(t, N);
    }
    else return Hit::NO_HIT();// This means that there is a line intersection but not a ray intersection.
}

Triangle::Triangle(Point const &pos1, Point const &pos2, Point const &pos3)
    : v1(pos1),
      v2(pos2),
      v3(pos3),
      N((v2 - v1).cross(v3 - v1))
{}
