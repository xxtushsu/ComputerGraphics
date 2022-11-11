#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    double a = ray.D.dot(ray.D);
    double b = 2 * ray.D.dot(ray.O - position);
    double c = (ray.O - position).dot(ray.O - position) - (r * r);
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return Hit::NO_HIT();

    Vector N;
    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);
    if (t1 < 0 && t2 < 0) return Hit::NO_HIT();
    if (t1 < 0 || t2 < 0) return (t1 > t2) ? Hit(t1, N) : Hit(t2, N);
    return (t1 < t2) ? Hit(t1, N) : Hit(t2, N);
    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, position, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ***************************************************/
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
