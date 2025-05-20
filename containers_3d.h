#ifndef PRIMITIVES_3D_H
#define PRIMITIVES_3D_H

struct Vector2 {
    float x;
    float y;
};

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Face {
    int vertex_indices[3];
    int texture_coordinate_indices[3];
    int normal_indices[3];
};

#endif
