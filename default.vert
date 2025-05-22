#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 frag_world_position;

void main() {
    gl_Position = projection * view * model * vec4(in_position, 1.0);

    mat3 normal_matrix = mat3(transpose(inverse(model)));
    normal = normal_matrix * in_normal;

    frag_world_position = (model * vec4(in_position, 1.0)).xyz;
}
