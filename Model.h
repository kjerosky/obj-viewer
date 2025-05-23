#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glm/glm.hpp>

#include "Face.h"

struct ModelStatistics {
    int vertex_count;
    int normal_count;
    int texture_coordinate_count;
    int face_count;
};

struct ModelExtents {
    glm::vec3 min;
    glm::vec3 max;
};

class Model {

public:

    Model();
    ~Model();

    void add_vertex(glm::vec3& vertex);
    void add_normal(glm::vec3& normal);
    void add_texture_coordinate(glm::vec2& texture_coordinate);
    void add_face(Face& face);

    float* get_buffer_data(int& size_in_bytes, int& vertex_count);
    ModelStatistics get_statistics();
    ModelExtents get_extents();

private:

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texture_coordinates;
    std::vector<Face> faces;
};

#endif
