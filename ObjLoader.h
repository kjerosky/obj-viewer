#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <optional>
#include <vector>
#include <string>

#include "Model.h"
#include "containers_3d.h"

class ObjLoader {

public:

    ObjLoader();
    ~ObjLoader();

    std::optional<Model> load_from_file(const std::string& file_path);

private:

    std::vector<std::string> split_string_by_whitespace(const std::string& str);
    std::vector<std::string> split_string_by_character(const std::string& str, const char delimiter);

    Face create_face_from_face_line_tokens(const std::vector<std::string>& face_line_tokens);
};

#endif
