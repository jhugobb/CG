#include "raytracer.h"

#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/plane.h"
#include "objloader.h"

// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parametrers ------------------------------
// =============================================================================

    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
        double radius = node["radius"];
        obj = ObjectPtr(new Sphere(pos, radius));
    } else if (node["type"] == "triangle") {
        json points = node["points"];
        Point pointA(points["a"]);
        Point pointB(points["b"]);
        Point pointC(points["c"]);
        obj = ObjectPtr(new Triangle(pointA, pointB, pointC));
    }
    else if (node["type"] == "plane") {
        Point p0(node["p0"]);
        Triple N(node["normal"]);
        obj = ObjectPtr(new Plane(p0, N));
    } else {
            cerr << "Unknown object type: " << node["type"] << ".\n";
    }

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->material = parseMaterialNode(node["material"]);
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node) const
{
    Color color(node["color"]);
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];
    return Material(color, ka, kd, ks, n);
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);

    // TODO: add your other configuration settings here
    ObjectPtr obj = nullptr;
    
    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    for (auto const &meshNode : jsonscene["Meshes"]) {
        const string s = meshNode["model"];
        OBJLoader objLoader = OBJLoader(s);
        vector<Vertex> vv = objLoader.vertex_data();
        for (unsigned int i = 0; i < vv.size(); i += 3){
            cout << vv[i].x << endl;
            Point p[3];
            Point translation(meshNode["translation"]);
            double scale(meshNode["scale"]);
            for (unsigned int j = 0; j < 3; j++){
                p[j].x = vv[i+j].x * scale + translation.x;
                p[j].y = vv[i+j].y * scale + translation.y;
                p[j].z = vv[i+j].z * scale + translation.z;
            }
            Triangle *t = new Triangle(p[0], p[1], p[2]);
            obj = ObjectPtr(t);
            obj->material = parseMaterialNode(meshNode["material"]);
            scene.addObject(obj);
        }
    }

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex)
{
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname)
{
    // TODO: the size may be a settings in your file
    Image img(400, 400);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
