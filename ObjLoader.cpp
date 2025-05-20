#include "ObjLoader.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "Model.h"
#include "primitives_3d.h"

ObjLoader::ObjLoader() {
    // do nothing for now
}

// --------------------------------------------------------------------------

ObjLoader::~ObjLoader() {
    // do nothing for now
}

// --------------------------------------------------------------------------

std::optional<Model> ObjLoader::load_from_file(const char* filename) {
    std::ifstream file(filename);
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

            Vertex vertex;
            vertex.x = std::stof(tokens[1]);
            vertex.y = std::stof(tokens[2]);
            vertex.z = std::stof(tokens[3]);

            model.add_vertex(vertex);
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
