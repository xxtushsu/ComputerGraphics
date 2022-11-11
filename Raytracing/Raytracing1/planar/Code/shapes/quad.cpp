#include "quad.h"

#include <limits>

using namespace std;

Hit Quad::intersect(Ray const &ray)
{
    if (triangle1->intersect(ray).t < numeric_limits<double>::infinity()) return triangle1->intersect(ray);
    else if (triangle2->intersect(ray).t < numeric_limits<double>::infinity()) return triangle2->intersect(ray);
    return Hit::NO_HIT();
}

Quad::Quad(Point const &v0,
           Point const &v1,
           Point const &v2,
           Point const &v3)
{
    triangle1 = ObjectPtr(new Triangle(v0, v1, v2));
    triangle2 = ObjectPtr(new Triangle(v0, v2, v3));
}
