#include "Model.h"

#include <iostream>

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
    // For now, we'll provide a very crude vertex-only buffer data array for triangle faces.

    int buffer_data_size = faces.size() * 3 * 3; // 3 vertices per face, 3 floats per vertex
    float* buffer_data = new float[buffer_data_size];

    int buffer_data_index = 0;
    for (Face face : faces) {
        for (int i = 0; i < 3; i++) {
            int vertex_index = face.vertex_indices[i];
            buffer_data[buffer_data_index++] = vertices[vertex_index].x;
            buffer_data[buffer_data_index++] = vertices[vertex_index].y;
            buffer_data[buffer_data_index++] = vertices[vertex_index].z;
        }
    }

    size_in_bytes = buffer_data_size * sizeof(float);
    vertex_count = faces.size() * 3;
    return buffer_data;
}

// --------------------------------------------------------------------------

void Model::print_debug_info() {
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Normals: " << normals.size() << std::endl;
    std::cout << "Texture coordinates: " << texture_coordinates.size() << std::endl;
    std::cout << "Faces: " << faces.size() << std::endl;
}
