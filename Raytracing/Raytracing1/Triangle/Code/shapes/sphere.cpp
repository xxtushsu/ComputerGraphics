#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    double a = ray.D.dot(ray.D);
    double b = 2 * ray.D.dot(ray.O - position);
    double c = (ray.O - position).dot(ray.O - position) - (r * r);
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return Hit::NO_HIT();  // No intersection

    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);

    double t;
    Vector N;
    if (t1 < 0 && t2 < 0) return Hit::NO_HIT();  // sphere behind camera
    else if (t1 < 0 || t2 < 0) {  // camera inside sphere
        t = max(t1, t2);
        N = -((ray.O + ray.D * t) - position).normalized();
    } else {  // sphere in front of camera
        t = min(t1, t2);
        N = ((ray.O + ray.D * t) - position).normalized();
    }

    return Hit(t, N);
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
