#version 330 core

in vec3 normal;
in vec3 frag_world_position;

uniform vec3 camera_world_position;
uniform vec3 sun_direction;
uniform vec3 ambient_light;
uniform vec3 base_color;
uniform float shininess;

out vec4 frag_color;

void main() {
    vec3 N = normalize(normal);
    vec3 L = normalize(-sun_direction);
    vec3 V = normalize(camera_world_position - frag_world_position);
    vec3 H = normalize(L + V);

    vec3 ambient = ambient_light * base_color;

    vec3 diffuse = max(dot(N, L), 0.0) * base_color;

    float specular_angle = max(dot(N, H), 0.0);
    float specular_amount = pow(specular_angle, shininess);
    vec3 specular = specular_amount * vec3(1.0) * vec3(1.0);

    vec3 final_color = clamp(ambient + diffuse + specular, 0.0, 1.0);

    frag_color = vec4(final_color, 1.0);
}
