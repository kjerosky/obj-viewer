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
#include "MouseHandler.h"

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

float calculate_initial_camera_distance_to_object(glm::vec3& dimensions, float fovx, float fovy) {
    float longest_obj_xz_distance_to_origin = glm::sqrt(0.5 * (dimensions.x * dimensions.x + dimensions.z * dimensions.z));

    float best_vertical_distance = dimensions.y / (2.0f * glm::tan(fovy / 2.0f)) + longest_obj_xz_distance_to_origin;
    float best_horizontal_distance = glm::max(dimensions.x, dimensions.z) / (2.0f * glm::tan(fovx / 2.0f)) + longest_obj_xz_distance_to_origin;

    return glm::max(best_vertical_distance, best_horizontal_distance);
}

// --------------------------------------------------------------------------

struct WindowResizeChanges {
    float rotation_degrees_per_pixel;
    float aspect_ratio;
};

WindowResizeChanges handle_window_resize(int new_window_width, int new_window_height) {
    glViewport(0, 0, new_window_width, new_window_height);

    WindowResizeChanges window_resize_changes;
    window_resize_changes.rotation_degrees_per_pixel = 360.0f / glm::max(new_window_width, new_window_height);
    window_resize_changes.aspect_ratio = (float)new_window_width / new_window_height;

    return window_resize_changes;
}

// --------------------------------------------------------------------------

int main(int argc, char** argv) {
    const int INITIAL_WINDOW_WIDTH = 500;
    const int INITIAL_WINDOW_HEIGHT = 500;

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
    std::cout << std::endl;
    std::cout << "Vertices: " << statistics.vertex_count << std::endl;
    std::cout << "Normals: " << statistics.normal_count << std::endl;
    std::cout << "Texture coordinates: " << statistics.texture_coordinate_count << std::endl;
    std::cout << "Faces: " << statistics.face_count << std::endl;
    std::cout << std::endl;

    ModelExtents extents = model.get_extents();
    std::cout << "Extents: " << glm::to_string(extents.min) << " => " << glm::to_string(extents.max) << std::endl;

    glm::vec3 dimensions = extents.max - extents.min;
    std::cout << "Dimensions: " << glm::to_string(dimensions) << std::endl;

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

    SDL_Window* window = SDL_CreateWindow(window_title.c_str(), INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    glm::vec3 sun_direction = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));
    glm::vec3 base_color = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 centered_model_translation = glm::translate(glm::mat4(1.0), -0.5f * (extents.min + extents.max));
    float rotation_degrees_x = 0.0f;
    float rotation_degrees_y = 0.0f;

    WindowResizeChanges window_resize_changes = handle_window_resize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    float rotation_degrees_per_pixel = window_resize_changes.rotation_degrees_per_pixel;
    float aspect_ratio = window_resize_changes.aspect_ratio;

    const float FOV_Y = glm::radians(45.0f);
    const float FOV_X = 2.0f * glm::atan(glm::tan(FOV_Y / 2.0f) * aspect_ratio);
    const float NEAR_CLIP_PLANE_DISTANCE = 0.1f;
    const float FAR_CLIP_PLANE_DISTANCE = 100.0f;

    float initial_camera_z = calculate_initial_camera_distance_to_object(dimensions, FOV_X, FOV_Y) + NEAR_CLIP_PLANE_DISTANCE;
    glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, initial_camera_z);

    glEnable(GL_DEPTH_TEST);

    const float DISTANCE_PER_MOUSE_WHEEL = 0.1f;

    MouseHandler mouse_handler(window);

    bool running = true;
    SDL_Event event;
    while (running) {
        glm::vec2 mouse_drag_motion = glm::vec2(0.0f, 0.0f);
        float mouse_wheel_motion = 0.0f;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                mouse_handler.handle_left_button_status(true);
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
                mouse_handler.handle_left_button_status(false);
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                mouse_drag_motion = mouse_handler.handle_mouse_motion(event.motion.xrel, event.motion.yrel);
            } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                mouse_wheel_motion = event.wheel.y;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                int new_window_width = event.window.data1;
                int new_window_height = event.window.data2;

                WindowResizeChanges window_resize_changes = handle_window_resize(new_window_width, new_window_height);
                rotation_degrees_per_pixel = window_resize_changes.rotation_degrees_per_pixel;
                aspect_ratio = window_resize_changes.aspect_ratio;
            }
        }

        camera_position.z += mouse_wheel_motion * DISTANCE_PER_MOUSE_WHEEL;

        rotation_degrees_x += mouse_drag_motion.y * rotation_degrees_per_pixel;
        rotation_degrees_y += mouse_drag_motion.x * rotation_degrees_per_pixel;

        glm::mat4 rotation_x = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degrees_x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotation_y = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degrees_y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model_matrix = rotation_x * rotation_y * centered_model_translation;

        glm::mat4 view_matrix = glm::lookAt(
            camera_position,
            camera_position + glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 projection_matrix = glm::perspective(
            FOV_Y,
            aspect_ratio,
            NEAR_CLIP_PLANE_DISTANCE,
            FAR_CLIP_PLANE_DISTANCE
        );

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_matrix));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view_matrix));
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection_matrix));
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
