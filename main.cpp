#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "ObjLoader.h"

bool read_file_into_string(const char* file_path, std::string& str) {
    std::ifstream file(file_path);
    if (!file) {
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    str = ss.str();

    return true;
}

// --------------------------------------------------------------------------

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

// --------------------------------------------------------------------------

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

// --------------------------------------------------------------------------

int main(int argc, char** argv) {
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 500;

    const char* vertex_shader_file_path = "default.vert";
    const char* fragment_shader_file_path = "default.frag";

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

    ModelStatistics statistics = model.get_statistics();
    std::cout << "Vertices: " << statistics.vertex_count << std::endl;
    std::cout << "Normals: " << statistics.normal_count << std::endl;
    std::cout << "Texture coordinates: " << statistics.texture_coordinate_count << std::endl;
    std::cout << "Faces: " << statistics.face_count << std::endl;
    std::cout << std::endl;

    ModelExtents extents = model.get_extents();
    std::cout << "Extents: " << glm::to_string(extents.min) << " => " << glm::to_string(extents.max) << std::endl;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return EXIT_FAILURE;
    }

    std::string vertex_shader_source_string;
    if (!read_file_into_string(vertex_shader_file_path, vertex_shader_source_string)) {
        std::cerr << "[ERROR] Could not open vertex shader file \"" << vertex_shader_file_path << "\"" << std::endl;
        return EXIT_FAILURE;
    }
    const char* vertex_shader_source = vertex_shader_source_string.c_str();

    std::string fragment_shader_source_string;
    if (!read_file_into_string(fragment_shader_file_path, fragment_shader_source_string)) {
        std::cerr << "[ERROR] Could not open fragment shader file \"" << fragment_shader_file_path << "\"" << std::endl;
        return EXIT_FAILURE;
    }
    const char* fragment_shader_source = fragment_shader_source_string.c_str();

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

    SDL_Window* window = SDL_CreateWindow(window_title.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    SDL_GL_SetSwapInterval(1);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLint model_location = glGetUniformLocation(shader_program, "model");
    GLint view_location = glGetUniformLocation(shader_program, "view");
    GLint projection_location = glGetUniformLocation(shader_program, "projection");
    GLint camera_world_position_location = glGetUniformLocation(shader_program, "camera_world_position");
    GLint sun_direction_location = glGetUniformLocation(shader_program, "sun_direction");
    GLint ambient_light_location = glGetUniformLocation(shader_program, "ambient_light");
    GLint base_color_location = glGetUniformLocation(shader_program, "base_color");
    GLint shininess_location = glGetUniformLocation(shader_program, "shininess");

    glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 sun_direction = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));
    glm::vec3 base_color = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 centered_model_translation = glm::translate(glm::mat4(1.0), -0.5f * (extents.min + extents.max));
    float y_rotation_degrees = 0.0f;

    glEnable(GL_DEPTH_TEST);

    Uint64 previous_time = SDL_GetTicksNS();

    const float ROTATION_DEGREES_PER_SECOND = 360.0f / 8.0f;

    bool running = true;
    SDL_Event event;
    while (running) {
        Uint64 current_time = SDL_GetTicksNS();
        float delta_time = (current_time - previous_time) / 1e9f;
        previous_time = current_time;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
            }
        }

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(y_rotation_degrees), glm::vec3(0.0f, 1.0f, 0.0f)) * centered_model_translation;
        y_rotation_degrees += ROTATION_DEGREES_PER_SECOND * delta_time;

        glm::mat4 view = glm::lookAt(
            camera_position,
            camera_position + glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            ((float)WINDOW_WIDTH) / WINDOW_HEIGHT,
            0.1f,
            100.0f
        );

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(camera_world_position_location, 1, glm::value_ptr(camera_position));
        glUniform3fv(sun_direction_location, 1, glm::value_ptr(sun_direction));
        glUniform3f(ambient_light_location, 0.2f, 0.2f, 0.2f);
        glUniform3fv(base_color_location, 1, glm::value_ptr(base_color));
        glUniform1f(shininess_location, 32.0f);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
