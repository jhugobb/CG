#include "objloader.h"

// Pro C++ Tip: here you can specify other includes you may need
// such as <iostream>

#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

using namespace std;

// ===================================================================
// -- Constructors and destructor ------------------------------------
// ===================================================================

// --- Public --------------------------------------------------------

OBJLoader::OBJLoader(string const &filename)
:
    d_hasTexCoords(false)
{
    parseFile(filename);
}

// ===================================================================
// -- Member functions -----------------------------------------------
// ===================================================================

// --- Public --------------------------------------------------------

vector<Vertex> OBJLoader::vertex_data() const
{
    vector<Vertex> data;

    // For all vertices in the model, interleave the data
    for (Vertex_idx const &vertex : d_vertices)
    {
        // Add coordinate data
        Vertex vert;

        vec3 const coord = d_coordinates.at(vertex.d_coord);
        vert.x = coord.x;
        vert.y = coord.y;
        vert.z = coord.z;

        // Add normal data
        vec3 const norm = d_normals.at(vertex.d_norm);
        vert.nx = norm.x;
        vert.ny = norm.y;
        vert.nz = norm.z;

        // Add texture data (if available)
        if (d_hasTexCoords)
        {
            vec2 const tex = d_texCoords.at(vertex.d_tex);
            vert.u = tex.u;      // u coordinate
            vert.v = tex.v;      // v coordinate
        } else {
            vert.u = 0;
            vert.v = 0;
        }
        data.push_back(vert);
    }

    return data;    // copy elision
}

unsigned OBJLoader::numTriangles() const
{
    return d_vertices.size() / 3U;
}

bool OBJLoader::hasTexCoords() const
{
    return d_hasTexCoords;
}

void OBJLoader::unitize()
{
    // This is a very handy function for importing models
    // which you may reuse in other projects.
    // You may have noticed you can use arbitrary sizes for your
    // models. You may find that modelers do not always use the
    // same size for models. Therefore it might be preferable to
    // scale the object to fit inside the unit cube so you can easily
    // set the right scale of your model in OpenGL. Aditionally,
    // the model does not have to be centered around the origin
    // (0, 0, 0) which may cause troubles when translating
    // This function should fix that!

    // A common approach looks like this:

    // Determine min / max and offset in each dimension
    // Determine by which factor to scale (largest difference
    //  in min / max in a dimension (Important! Scale uniformaly in
    //  all dimensions!)
    // Loop over all coordinate data and scale the coordinates
    //  and apply the translate/scaling
    vector<Vertex> v = this->vertex_data();
    unsigned int size = v.size();
    double scale = 2.0 / getMaxLength(v, static_cast <double> (size));

    #pragma omp parallel for 
    for (unsigned int i = 0; i < size; i++) {
        v[i].x *= scale;
        v[i].y *= scale;
        v[i].z *= scale;
    }

}

// --- Private -------------------------------------------------------
/**
 * @brief getXLength Calculates the length of the element (assuming the vertexes composes an element) from the X axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return X Length
 */
float OBJLoader::getXLength(vector<Vertex> v, int N)
{
    float min = std::numeric_limits<float>::max(), max = -std::numeric_limits<float>::max();
    for (int i = 0; i < N; i++)
    {
        if (v[i].x < min)
        {
            min = v[i].x;
        }
        else if (v[i].x > max)
        {
            max = v[i].x;
        }
    }
    return (max - min);
}

/**
 * @brief getYLength Calculates the length of the element (assuming the vertexes composes an element) from the Z axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return Y Length
 */
float OBJLoader::getYLength(vector<Vertex> v, int N)
{
    float min = std::numeric_limits<float>::max(), max = -std::numeric_limits<float>::max();
    for (int i = 0; i < N; i++)
    {
        if (v[i].y < min)
        {
            min = v[i].y;
        }
        else if (v[i].y > max)
        {
            max = v[i].y;
        }
    }
    return (max - min);
}

/**
 * @brief getZLength Calculates the length of the element (assuming the vertexes composes an element) from the Z axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return Z Length
 */
float OBJLoader::getZLength(vector<Vertex> v, int N)
{
    float min = std::numeric_limits<float>::max(), max = -std::numeric_limits<float>::max();
    for (int i = 0; i < N; i++)
    {
        if (v[i].z < min)
        {
            min = v[i].z;
        }
        else if (v[i].z > max)
        {
            max = v[i].z;
        }
    }
    return (max - min);
}

/**
 * @brief getMaxLength Returns the greatest length from the 3 axis perspective of a model.
 * @param v Model.
 * @param N Number of vertexes.
 * @return Greatest length.
 */
float OBJLoader::getMaxLength(vector<Vertex> v, int N)
{
    float max = getXLength(v, N), tmp;

    if ((tmp = getYLength(v, N)) > max)
    {
        max = tmp;
    }

    if ((tmp = getZLength(v, N)) > max)
    {
        max = tmp;
    }

    return max;
}

void OBJLoader::parseFile(string const &filename)
{
    ifstream file(filename);
    if (file)
    {
        string line;
        while(getline(file, line))
            parseLine(line);

    } else {
        cerr << "Could not open: " << filename << " for reading!\n";
    }
}

void OBJLoader::parseLine(string const &line)
{
    if (line[0] == '#')
        return;                     // ignore comments

    StringList tokens = split(line, ' ', false);

    if (tokens[0] == "v")
        parseVertex(tokens);
    else if (tokens[0] == "vn")
        parseNormal(tokens);
    else if (tokens[0] == "vt")
        parseTexCoord(tokens);
    else if (tokens[0] == "f")
        parseFace(tokens);

    // Other data is also ignored
}

void OBJLoader::parseVertex(StringList const &tokens)
{
    float x, y, z;
    x = stof(tokens.at(1));         // 0 is the "v" token
    y = stof(tokens.at(2));
    z = stof(tokens.at(3));
    d_coordinates.push_back(vec3{x, y, z});
}

void OBJLoader::parseNormal(StringList const &tokens)
{
    float x, y, z;
    x = stof(tokens.at(1));         // 0 is the "vn" token
    y = stof(tokens.at(2));
    z = stof(tokens.at(3));
    d_normals.push_back(vec3{x, y, z});
}

void OBJLoader::parseTexCoord(StringList const &tokens)
{
    d_hasTexCoords = true;          // Texture data will be read

    float u, v;
    u = stof(tokens.at(1));         // 0 is the "vt" token
    v = stof(tokens.at(2));
    d_texCoords.push_back(vec2{u, v});
}

void OBJLoader::parseFace(StringList const &tokens)
{
    // skip the first token ("f")
    for (size_t idx = 1; idx < tokens.size(); ++idx)
    {
        // format is:
        // <vertex idx + 1>/<texture idx +1>/<normal idx + 1>
        // Wavefront .obj files start counting from 1 (yuck)

        StringList elements = split(tokens.at(idx), '/');
        Vertex_idx vertex {}; // initialize to zeros on all fields

        vertex.d_coord = stoul(elements.at(0)) - 1U;

        if (d_hasTexCoords)
            vertex.d_tex = stoul(elements.at(1)) - 1U;
        else
            vertex.d_tex = 0U;       // ignored

        vertex.d_norm = stoul(elements.at(2)) - 1U;

        d_vertices.push_back(vertex);
    }
}

OBJLoader::StringList OBJLoader::split(string const &line,
                            char splitChar,
                            bool keepEmpty)
{
    StringList tokens;
    istringstream iss(line);
    string token;
    while (getline(iss, token, splitChar))
        if (token.size() > 0 || (token.size() == 0 && keepEmpty))
            tokens.push_back(token);

    return tokens;
}
