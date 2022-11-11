#include "triangle.h"
#include<cmath>

#define ESP 0.000001

Hit Triangle::intersect(Ray const &ray)
{
    if (fabs(ray.D.dot(N)) < ESP) return Hit::NO_HIT();  // ray parallel to triangle

    double t = N.dot(v0 - ray.O) / N.dot(ray.D);
    Point p = ray.O + ray.D * t;

    if (t < 0) return Hit::NO_HIT();  // triangle is behind camera

    double totalArea = this->getArea();
    double area1 = Triangle(p, v1, v2).getArea();
    double area2 = Triangle(v0, p, v2).getArea();
    double area3 = Triangle(v0, v1, p).getArea();
    bool isInside = (fabs(area1 + area2 + area3 - totalArea) < ESP);

    if (!isInside) return Hit::NO_HIT();  // intersection outside triangle.
    return (ray.D.dot(N) < 0) ? Hit(t, N) : Hit(t, -N);
}

double Triangle::getArea() {
    return fabs(((v1 - v0).cross(v2 - v0).length())/2.0);
}

Triangle::Triangle(Point const &v0, Point const &v1, Point const &v2):
    v0(v0), v1(v1), v2(v2), N()
{
    N = (v1 - v0).cross(v2 - v0);
    N.normalize();
}
