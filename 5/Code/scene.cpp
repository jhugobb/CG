#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>
#include <iostream>

using namespace std;

ObjectPtr Scene::intersectObj(Hit *min_hit, Ray ray, ObjectPtr obj) {
    ObjectPtr objtmp = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));

        if (objects[idx] != obj && hit.t < min_hit->t)
        {
            *min_hit = hit;
            objtmp = objects[idx];
        }
    }
    return objtmp;
}

Color Scene::trace(Ray const &ray, double recursionDepth)
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = intersectObj(&min_hit, ray, nullptr);

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material material = obj->material;          //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector


    Color IA, ID, IS;
    IA = ID = IS = Color();
    Vector L, R;
    ObjectPtr objLight = nullptr;
    Vector reflectionView = 2 * (V.dot(N)) * N - V;

    // If we want to calculate reflection
    if (recursionDepth > 0) {
        Ray reflectionRay(hit + 0.1 * reflectionView, reflectionView);
        IS = trace(reflectionRay, recursionDepth - 1);
    }

    for (unsigned int i = 0; i < lights.size(); i++) {
        L = (lights[i]->position - hit).normalized();
        Hit min_hit_light(numeric_limits<double>::infinity(), Vector());
        // If we want to take shadows into account 
        if (shadow) {
            Ray lightRay(hit + 0.001*L , L);
            objLight = intersectObj(&min_hit_light, lightRay, obj);
        }
        // If we didnt hit any other object -> always true when we don't take shadows into account
        if (!objLight) {
            R = 2 * (L.dot(N)) * N - L;
            ID += max(0.0, L.dot(N)) * lights[i]->color;
            IS += pow(max(0.0, R.dot(V)), material.n) * lights[i]->color;
        }
    }
    IA = obj->getColor(hit) * material.ka;
    ID = ID * obj->getColor(hit) * material.kd;
    IS = IS * material.ks;

    Color color = IA + ID + IS;
    
    return color;
}

Color Scene::superSample(unsigned x, unsigned y, unsigned h, double n) {
    // if n = 4 => 4x4 super sampling
    Color finalColor;
    for (double iy = (1 / (2 * n)); iy < 1; iy += (1 / n)) {
        for (double ix = (1 / (2 * n)); ix < 1; ix += (1 / n)) {
            Point pixel(x + ix, h - 1 - y + iy, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color c = trace(ray, recursionDepth);
            finalColor += c;
        }
    }
    return finalColor/(n*n);
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();
    #pragma omp parallel for
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            // Currently 2x2 Anti-aliasing, but implemented to be nxn
            Color finalColor = superSample(x, y, h, 1);
            finalColor.clamp();
            img(x, y) = finalColor;
        }
    }
}

// --- Misc functions ----------------------------------------------------------

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

void Scene::setShadow(bool const &isShadowActive) {
    shadow = isShadowActive;
}

void Scene::setRecursionDepth(double const &value) {
    recursionDepth = value;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}
