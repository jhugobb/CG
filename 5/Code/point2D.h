#ifndef POINT2D_H_
#define POINT2D_H_

class Point2D;
class Point2D
{
  public:
    double x;
    double y;

    Point2D(double const &givenx, double const &giveny)
      :
      x(givenx),
      y(giveny)
    {}
};

#endif