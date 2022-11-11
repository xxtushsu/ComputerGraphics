#include "mesh.h"

#include "../objloader.h"
#include "../vertex.h"
#include "triangle.h"

#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

Hit Mesh::intersect(Ray const &ray)
{
    // Find hit triangle and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    for (auto & tri : d_tris) {
        Hit hit(tri->intersect(ray));
        if (hit.t < min_hit.t) min_hit = hit;
    }

    return min_hit;
}

Mesh::Mesh(string const &filename, Point const &position, Vector const &rotation, Vector const &scale)
{
    OBJLoader model(filename);
    d_tris.reserve(model.numTriangles());
    vector<Vertex> vertices = model.vertex_data();
    for (size_t tri = 0; tri != model.numTriangles(); ++tri)
    {
        Vertex one = vertices[tri * 3];
        Point v0(one.x, one.y, one.z);

        Vertex two = vertices[tri * 3 + 1];
        Point v1(two.x, two.y, two.z);

        Vertex three = vertices[tri * 3 + 2];
        Point v2(three.x, three.y, three.z);

        // Non-uniform scaling
        v0 = Point(v0.x * scale.x, v0.y * scale.y, v0.z * scale.z);
        v1 = Point(v1.x * scale.x, v1.y * scale.y, v1.z * scale.z);
        v2 = Point(v2.x * scale.x, v2.y * scale.y, v2.z * scale.z);

        // Rotation x
        v0 = Point(v0.x, v0.y * cos(rotation.x) - v0.z * sin(rotation.x), v0.y * sin(rotation.x) + v0.z * cos(rotation.x));
        v1 = Point(v1.x, v1.y * cos(rotation.x) - v1.z * sin(rotation.x), v1.y * sin(rotation.x) + v1.z * cos(rotation.x));
        v2 = Point(v2.x, v2.y * cos(rotation.x) - v2.z * sin(rotation.x), v2.y * sin(rotation.x) + v2.z * cos(rotation.x));
        // Rotation y
        v0 = Point(v0.x * cos(rotation.y) + v0.z * sin(rotation.y), v0.y, -v0.x * sin(rotation.y) + v0.z * cos(rotation.y));
        v1 = Point(v1.x * cos(rotation.y) + v1.z * sin(rotation.y), v1.y, -v1.x * sin(rotation.y) + v1.z * cos(rotation.y));
        v2 = Point(v2.x * cos(rotation.y) + v2.z * sin(rotation.y), v2.y, -v2.x * sin(rotation.y) + v2.z * cos(rotation.y));
        // Rotation z
        v0 = Point(v0.x * cos(rotation.z) - v0.y * sin(rotation.z), v0.x * sin(rotation.z) + v0.y * cos(rotation.z), v0.z);
        v1 = Point(v1.x * cos(rotation.z) - v1.y * sin(rotation.z), v1.x * sin(rotation.z) + v1.y * cos(rotation.z), v1.z);
        v2 = Point(v2.x * cos(rotation.z) - v2.y * sin(rotation.z), v2.x * sin(rotation.z) + v2.y * cos(rotation.z), v2.z);

        // Translation
        v0 = Point(v0.x + position.x, v0.y + position.y, v0.z + position.z);
        v1 = Point(v1.x + position.x, v1.y + position.y, v1.z + position.z);
        v2 = Point(v2.x + position.x, v2.y + position.y, v2.z + position.z);

        d_tris.push_back(ObjectPtr(new Triangle(v0, v1, v2)));
    }

    cout << "Loaded model: " << filename << " with " <<
        model.numTriangles() << " triangles.\n";
}
