#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "containers_3d.h"

class Model {

public:

    Model();
    ~Model();

    void add_vertex(Vector3& vertex);
    void add_normal(Vector3& normal);
    void add_texture_coordinate(Vector2& texture_coordinate);

    void print_debug_info();

private:

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texture_coordinates;
};

#endif
