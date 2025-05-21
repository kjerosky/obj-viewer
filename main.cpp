#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <iostream>
#include <stdlib.h>

#include "ObjLoader.h"

const char* vertex_shader_source = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 in_pos;

    void main() {
        gl_Position = vec4(in_pos, 1.0);
    }
)glsl";

const char* fragment_shader_source = R"glsl(
    #version 330 core
    out vec4 frag_color;

    void main() {
        frag_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
)glsl";

bool check_shader_compilation(GLuint shader, const std::string& shader_type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << shader_type << "shader compilation failed:\n" << infoLog << "\n";
        return false;
    }
    return true;
}

bool check_program_linkage(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking failed:\n" << infoLog << "\n";
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    std::string program_name = argv[0];
    if (argc != 2) {
        std::cerr << "Usage: " << program_name << " OBJ_FILE" << std::endl;
        return EXIT_FAILURE;
    }

    std::string file_path = argv[1];

    ObjLoader obj_loader;
    std::optional<Model> loaded_model = obj_loader.load_from_file(file_path);
    if (!loaded_model.has_value()) {
        std::cerr << "[ERROR] Could not open file \"" << file_path << "\"" << std::endl;
        return EXIT_FAILURE;
    }

    Model model = loaded_model.value();
    model.print_debug_info();

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

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    if (!check_shader_compilation(vertex_shader, "vertex")) {
        return EXIT_FAILURE;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    if (!check_shader_compilation(fragment_shader, "fragment")) {
        return EXIT_FAILURE;
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    if (!check_program_linkage(shader_program)) {
        return EXIT_FAILURE;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    int buffer_data_size_in_bytes, vertex_count;
    float* buffer_data = model.get_buffer_data(buffer_data_size_in_bytes, vertex_count);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer_data_size_in_bytes, buffer_data, GL_STATIC_DRAW);
    delete[] buffer_data;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

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

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
