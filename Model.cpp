#include "Model.h"

#include <limits>
#include <algorithm>

Model::Model() {
    // do nothing for now
}

// --------------------------------------------------------------------------

Model::~Model() {
    // do nothing for now
}

// --------------------------------------------------------------------------

void Model::add_vertex(Vector3& vertex) {
    vertices.push_back(vertex);
}

// --------------------------------------------------------------------------

void Model::add_normal(Vector3& normal) {
    normals.push_back(normal);
}

// --------------------------------------------------------------------------

void Model::add_texture_coordinate(Vector2& texture_coordinate) {
    texture_coordinates.push_back(texture_coordinate);
}

// --------------------------------------------------------------------------

void Model::add_face(Face& face) {
    faces.push_back(face);
}

// --------------------------------------------------------------------------

float* Model::get_buffer_data(int& size_in_bytes, int& vertex_count) {
    // For now, we'll only provide vertices and normals in the buffer data data for triangle faces.

    int buffer_data_size = faces.size() * 3 * 6; // 3 vertices per face, 6 floats per vertex
    float* buffer_data = new float[buffer_data_size];

    int buffer_data_index = 0;
    for (Face face : faces) {
        for (int i = 0; i < 3; i++) {
            int vertex_index = face.vertex_indices[i];
            buffer_data[buffer_data_index++] = vertices[vertex_index].x;
            buffer_data[buffer_data_index++] = vertices[vertex_index].y;
            buffer_data[buffer_data_index++] = vertices[vertex_index].z;

            int normal_index = face.normal_indices[i];
            buffer_data[buffer_data_index++] = normals[normal_index].x;
            buffer_data[buffer_data_index++] = normals[normal_index].y;
            buffer_data[buffer_data_index++] = normals[normal_index].z;
        }
    }

    size_in_bytes = buffer_data_size * sizeof(float);
    vertex_count = faces.size() * 3;
    return buffer_data;
}

// --------------------------------------------------------------------------

ModelStatistics Model::get_statistics() {
    ModelStatistics statistics;
    statistics.vertex_count = vertices.size();
    statistics.normal_count = normals.size();
    statistics.texture_coordinate_count = texture_coordinates.size();
    statistics.face_count = faces.size();

    return statistics;
}

// --------------------------------------------------------------------------

ModelExtents Model::get_extents() {
    const float MIN_FLOAT_VALUE = std::numeric_limits<float>::min();
    const float MAX_FLOAT_VALUE = std::numeric_limits<float>::max();

    ModelExtents extents;
    extents.min = glm::vec3(MAX_FLOAT_VALUE, MAX_FLOAT_VALUE, MAX_FLOAT_VALUE);
    extents.max = glm::vec3(MIN_FLOAT_VALUE, MIN_FLOAT_VALUE, MIN_FLOAT_VALUE);

    for (Vector3 vertex : vertices) {
        extents.min.x = std::min(extents.min.x, vertex.x);
        extents.min.y = std::min(extents.min.y, vertex.y);
        extents.min.z = std::min(extents.min.z, vertex.z);

        extents.max.x = std::max(extents.max.x, vertex.x);
        extents.max.y = std::max(extents.max.y, vertex.y);
        extents.max.z = std::max(extents.max.z, vertex.z);
    }

    return extents;
}
