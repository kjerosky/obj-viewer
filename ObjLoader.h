#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <optional>
#include <vector>
#include <string>

#include "Model.h"

class ObjLoader {

public:

    ObjLoader();
    ~ObjLoader();

    std::optional<Model> load_from_file(const char* filename);

private:

    std::vector<std::string> split_string_by_whitespace(const std::string& str);
};

#endif
