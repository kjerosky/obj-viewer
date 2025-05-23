#ifndef FACE_H
#define FACE_H

struct Face {
    int vertex_indices[3];
    int texture_coordinate_indices[3];
    int normal_indices[3];
};

#endif
