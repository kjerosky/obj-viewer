#include <iostream>
#include <stdlib.h>

#include "ObjLoader.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OBJ_FILE" << std::endl;
        return EXIT_FAILURE;
    }

    ObjLoader obj_loader;
    std::optional<Model> model = obj_loader.load_from_file(argv[1]);
    if (!model.has_value()) {
        std::cerr << "[ERROR] Could not open file \"" << argv[1] << "\"" << std::endl;
        return 1;
    }

    model.value().print_debug_info();

    return 0;
}
