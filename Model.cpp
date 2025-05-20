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

void Model::print_debug_info() {
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Normals: " << normals.size() << std::endl;
    std::cout << "Texture coordinates: " << texture_coordinates.size() << std::endl;
}
