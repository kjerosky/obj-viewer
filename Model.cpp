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

void Model::add_vertex(Vertex& vertex) {
    vertices.push_back(vertex);
}

// --------------------------------------------------------------------------

void Model::print_debug_info() {
    std::cout << "Vertices: " << vertices.size() << std::endl;
}
