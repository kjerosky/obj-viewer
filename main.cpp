#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <iostream>
#include <stdlib.h>

#include "ObjLoader.h"

int main(int argc, char** argv) {
    std::string program_name = argv[0];
    if (argc != 2) {
        std::cerr << "Usage: " << program_name << " OBJ_FILE" << std::endl;
        return EXIT_FAILURE;
    }

    std::string file_path = argv[1];

    ObjLoader obj_loader;
    std::optional<Model> model = obj_loader.load_from_file(file_path);
    if (!model.has_value()) {
        std::cerr << "[ERROR] Could not open file \"" << file_path << "\"" << std::endl;
        return EXIT_FAILURE;
    }

    model.value().print_debug_info();

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    std::string window_title;
    size_t last_slash_in_file_path_index = file_path.find_last_of('/');
    if (last_slash_in_file_path_index == std::string::npos) {
        window_title = file_path;
    } else {
        window_title = file_path.substr(last_slash_in_file_path_index + 1);
    }
    window_title += " - OBJ Viewer";

    SDL_Window* window = SDL_CreateWindow(window_title.c_str(), 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "GLEW init error: " << glewGetErrorString(glewStatus) << "\n";
        return EXIT_FAILURE;
    }

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
