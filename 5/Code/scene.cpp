#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

Color Scene::lightTrace(const Material &material, Point hit, Vector N, Vector V, int objectID)
{
    Color IA, ID, IS;
    IA = ID = IS = Color();
    Vector L, R;
    if (!Shadows) //if the shadows are not activated
    {
        for (unsigned int i = 0; i < lights.size(); i++)
        {
            L = (lights[i]->position - hit).normalized();
            ID += max(0.0, L.dot(N)) * lights[i]->color;
            R = 2 * (L.dot(N)) * N - L;
            IS += pow(max(0.0, R.dot(V)), material.n) * lights[i]->color;
        }
    }
    else
    { //if the shadows are activated
        for (unsigned int i = 0; i < lights.size(); i++)
        {
            L = (lights[i]->position - hit).normalized();
            double min_hit_l = (lights[i]->position - hit).length();
            Ray lightRay = Ray(lights[i]->position, -L);

            bool noShadow = true;
            //checks if the light ray intersects any other object first
            for (int i2 = 0; i2 < objects.size() && noShadow; i2++)
            {
                Hit hittmp(objects[i2]->intersect(lightRay));
                if (i2 != objectID && hittmp.t  < min_hit_l)
                    noShadow = false;
            }

            if (noShadow)
            {
                ID += max(0.0, L.dot(N)) * lights[i]->color;
                R = 2 * (L.dot(N)) * N - L;
                IS += pow(max(0.0, R.dot(V)), material.n) * lights[i]->color;
            }
        }
    }

    IA = material.color * material.ka;
    ID = ID * material.color * material.kd;
    IS = IS * material.ks;

    //double ID = max(0.0, N.dot(V)) * material.kd;
    return IA + ID + IS;
}

Color Scene::trace(Ray const &ray)
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    int objID;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = objects[idx];
            objID = idx;
        }
    }

    // No hit? Return background color.
    if (!obj)
        return Color(0.0, 0.0, 0.0);

    /****************************************************
     * This is where you should insert the color
     * calculation (Phong model).
     *
     * Given: material, hit, N, V, lights[]
     * Sought: color
     *
     * Hints: (see triple.h)
     *        Triple.dot(Vector) dot product
     *        Vector + Vector    vector sum
     *        Vector - Vector    vector difference
     *        Point - Point      yields vector
     *        Vector.normalize() normalizes vector, returns length
     *        double * Color     scales each color component (r,g,b)
     *        Color * Color      dito
     *        pow(a,b)           a to the power of b
     ****************************************************/
    Material material = obj->material; //the hit objects material
    Point hit = ray.at(min_hit.t);     //the hit point
    Vector N = min_hit.N;              //the normal at hit point
    Vector V = -ray.D;                 //the view vector

    return lightTrace(material, hit, N, V, objID);
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
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x, y) = col;
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

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}

void Scene::setShadow(bool s)
{
    Shadows = s;
}
