#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include "primitives_3d.h"

class Model {

public:

    Model();
    ~Model();

    void add_vertex(Vertex& vertex);

    void print_debug_info();

private:

    std::vector<Vertex> vertices;
};

#endif
