#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "containers_3d.h"

struct ModelStatistics {
    int vertex_count;
    int normal_count;
    int texture_coordinate_count;
    int face_count;
};

class Model {

public:

    Model();
    ~Model();

    void add_vertex(Vector3& vertex);
    void add_normal(Vector3& normal);
    void add_texture_coordinate(Vector2& texture_coordinate);
    void add_face(Face& face);

    float* get_buffer_data(int& size_in_bytes, int& vertex_count);
    ModelStatistics get_statistics();

private:

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texture_coordinates;
    std::vector<Face> faces;
};

#endif
