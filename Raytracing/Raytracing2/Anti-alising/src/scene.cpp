#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

pair<ObjectPtr, Hit> Scene::castRay(Ray const &ray) const
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    return pair<ObjectPtr, Hit>(obj, min_hit);
}

Color Scene::trace(Ray const &ray, unsigned depth)
{
    pair<ObjectPtr, Hit> mainhit = castRay(ray);
    ObjectPtr obj = mainhit.first;
    Hit min_hit = mainhit.second;

    // No hit? Return background color.
    if (!obj)
        return Color(0.0, 0.0, 0.0);

    Material const &material = obj->material;
    Point hit = ray.at(min_hit.t);
    Vector V = -ray.D;

    // Pre-condition: For closed objects, N points outwards.
    Vector N = min_hit.N;

    // The shading normal always points in the direction of the view,
    // as required by the Phong illumination model.
    Vector shadingN;
    if (N.dot(V) >= 0.0)
        shadingN = N;
    else
        shadingN = -N;

    Color matColor = material.color;

    // Add ambient once, regardless of the number of lights.
    Color color = material.ka * matColor;

    // Add diffuse and specular components.
    for (auto const &light : lights) {
        Vector L = (light->position - hit).normalized();

        if (renderShadows) {
            // Shadow rendering.
            Ray shadowRay = Ray(hit + shadingN * epsilon, L);
            pair<ObjectPtr, Hit> shadowHit = castRay(shadowRay);

            // No object in hit by shadow ray.
            int inShadow = shadowHit.first != nullptr;
            // No object in between light scr and object.
            inShadow *= (shadowRay.at(shadowHit.second.t) - hit).length_2() < (light->position - hit).length_2();
            if (inShadow) continue;
        }

        // Add diffuse.
        double dotNormal = shadingN.dot(L);
        double diffuse = std::max(dotNormal, 0.0);
        color += diffuse * material.kd * light->color * matColor;

        // Add specular.
        if(dotNormal > 0)
        {
            Vector reflectDir = reflect(-L, shadingN); // Note: reflect(..) is not given in the framework.
            double specAngle = std::max(reflectDir.dot(V), 0.0);
            double specular = std::pow(specAngle, material.n);

            color += specular * material.ks * light->color;
        }
    }

    if (depth > 0 and material.isTransparent)
    {
        // When the ray is going into the material ni = air and nt = material, otherwise we swap.
        double ni, nt;
        if (N.dot(V) >= 0.0) {
            ni = 1.0;
            nt = material.nt;
        } else {
            ni = material.nt;
            nt = 1.0;
        }

        Vector T = ni * (ray.D - (ray.D.dot(shadingN)) * shadingN) / nt
                 - shadingN * sqrt(1 - pow(ni, 2) * (1 - pow(ray.D.dot(shadingN), 2))/pow(nt, 2));
        Ray refractionRay = Ray(hit - shadingN * epsilon, T);

        Vector R = 2 * (shadingN.dot(V)) * shadingN - V;
        Ray reflectionRay = Ray(hit + shadingN * epsilon, R);

        // Use Schlick's approximation to determine the ratio between the two.
        double kr0 = pow((ni - nt)/(ni + nt), 2);
        double kr = kr0 + (1 - kr0) * pow(1 - V.dot(shadingN), 5);
        double kt = 1 - kr;

        color += trace(refractionRay, depth - 1) * kt;
        color += trace(reflectionRay, depth - 1) * kr;
    }
    else if (depth > 0 and material.ks > 0.0)
    {
        // The object is not transparent, but opaque.
        Vector R = 2 * (shadingN.dot(V)) * shadingN - V;
        Ray reflectionRay = Ray(hit + shadingN * epsilon, R);
        color += trace(reflectionRay, depth - 1) * material.ks;
    }

    return color;
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();

    unsigned samples = supersamplingFactor * supersamplingFactor;

    for (unsigned y = 0; y < h; ++y)
        for (unsigned x = 0; x < w; ++x)
        {
            Color col = Color(0.0, 0.0, 0.0);
            for (unsigned n = 0; n < samples; n++) {
                float i = ((n % supersamplingFactor) + 1) / ((float) supersamplingFactor + 1.0f);
                float j = ((n / supersamplingFactor) + 1) / ((float) supersamplingFactor + 1.0f);
                Point pixel(x + i, h - 1 - y + j, 0);
                Ray ray(eye, (pixel - eye).normalized());
                col += trace(ray, recursionDepth) / samples;
            }

            col.clamp();
            img(x, y) = col;
        }
}

// --- Misc functions ----------------------------------------------------------

// Defaults
Scene::Scene()
:
    objects(),
    lights(),
    eye(),
    renderShadows(false),
    recursionDepth(0),
    supersamplingFactor(1)
{}

void Scene::addObject(ObjectPtr obj)
{
    objects.push_back(obj);
}

void Scene::addLight(Light const &light)
{
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position)
{
    eye = position;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}

void Scene::setRenderShadows(bool shadows)
{
    renderShadows = shadows;
}

void Scene::setRecursionDepth(unsigned depth)
{
    recursionDepth = depth;
}

void Scene::setSuperSample(unsigned factor)
{
    supersamplingFactor = factor;
}
