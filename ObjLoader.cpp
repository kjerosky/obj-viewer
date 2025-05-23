#include "ObjLoader.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

ObjLoader::ObjLoader() {
    // do nothing for now
}

// --------------------------------------------------------------------------

ObjLoader::~ObjLoader() {
    // do nothing for now
}

// --------------------------------------------------------------------------

std::optional<Model> ObjLoader::load_from_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file) {
        return std::nullopt;
    }

    Model model;

    std::string line;
    while (getline(file, line)) {
        size_t comment_start_position = line.find('#');
        if (comment_start_position != std::string::npos) {
            line = line.substr(0, comment_start_position);
        }

        std::vector<std::string> tokens = split_string_by_whitespace(line);
        if (tokens.size() == 0) {
            continue;
        } else if (tokens[0] == "v") {
            if (tokens.size() != 4) {
                std::cerr << "[ERROR] Vertex line contains unexpected number of tokens!" << std::endl;
                std::cerr << "[ERROR] Line content: " << line << std::endl;
                return std::nullopt;
            }

            glm::vec3 vertex;
            vertex.x = std::stof(tokens[1]);
            vertex.y = std::stof(tokens[2]);
            vertex.z = std::stof(tokens[3]);

            model.add_vertex(vertex);
        } else if (tokens[0] == "vn") {
            if (tokens.size() != 4) {
                std::cerr << "[ERROR] Normal line contains unexpected number of tokens!" << std::endl;
                std::cerr << "[ERROR] Line content: " << line << std::endl;
                return std::nullopt;
            }

            glm::vec3 normal;
            normal.x = std::stof(tokens[1]);
            normal.y = std::stof(tokens[2]);
            normal.z = std::stof(tokens[3]);

            model.add_normal(normal);
        } else if (tokens[0] == "vt") {
            if (tokens.size() != 3) {
                std::cerr << "[ERROR] Texture coordinate line contains unexpected number of tokens!" << std::endl;
                std::cerr << "[ERROR] Line content: " << line << std::endl;
                return std::nullopt;
            }

            glm::vec2 texture_coordinate;
            texture_coordinate.x = std::stof(tokens[1]);
            texture_coordinate.y = std::stof(tokens[2]);

            model.add_texture_coordinate(texture_coordinate);
        } else if (tokens[0] == "f") {
            if (tokens.size() != 4) {
                std::cerr << "[ERROR] Face line contains unexpected number of tokens!" << std::endl;
                std::cerr << "[ERROR] Please note that only triangles are currently supported." << std::endl;
                std::cerr << "[ERROR] Line content: " << line << std::endl;
                return std::nullopt;
            }

            Face face = create_face_from_face_line_tokens(tokens);
            model.add_face(face);
        } else {
            std::cerr << "[WARN] Ignoring unknown token: " << tokens[0] << std::endl;
        }
    }

    return model;
}

// --------------------------------------------------------------------------

std::vector<std::string> ObjLoader::split_string_by_whitespace(const std::string& str) {
    std::istringstream stream(str);
    std::vector<std::string> tokens;

    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

// --------------------------------------------------------------------------

std::vector<std::string> ObjLoader::split_string_by_character(const std::string& str, const char delimiter) {
    std::istringstream stream(str);
    std::vector<std::string> tokens;

    std::string token;
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

// --------------------------------------------------------------------------

Face ObjLoader::create_face_from_face_line_tokens(const std::vector<std::string>& face_line_tokens) {
    Face face;

    for (int i = 0; i < 3; i++) {
        std::string face_line_token = face_line_tokens[1 + i];
        std::vector<std::string> tokens = split_string_by_character(face_line_token, '/');

        face.vertex_indices[i] = std::stoi(tokens[0]) - 1;
        face.texture_coordinate_indices[i] = -1;
        face.normal_indices[i] = -1;

        if (tokens.size() >= 2 && tokens[1] != "") {
            face.texture_coordinate_indices[i] = std::stoi(tokens[1]) - 1;
        }

        if (tokens.size() >= 3 && tokens[2] != "") {
            face.normal_indices[i] = std::stoi(tokens[2]) - 1;
        }
    }

    return face;
}
