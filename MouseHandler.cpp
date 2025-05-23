#include "MouseHandler.h"

MouseHandler::MouseHandler(SDL_Window* window)
:
window(window),
is_left_button_down(false) {
    // do nothing for now
}

// --------------------------------------------------------------------------

MouseHandler::~MouseHandler() {
    // do nothing for now
}

// --------------------------------------------------------------------------

void MouseHandler::handle_left_button_status(bool is_button_down) {
    if (is_button_down == is_left_button_down) {
        return;
    }

    is_left_button_down = is_button_down;
    SDL_SetWindowRelativeMouseMode(window, is_left_button_down);
}

// --------------------------------------------------------------------------

glm::vec2 MouseHandler::handle_mouse_motion(float relative_x, float relative_y) {
    glm::vec2 mouse_drag_motion;
    if (is_left_button_down) {
        mouse_drag_motion.x = relative_x;
        mouse_drag_motion.y = relative_y;
    }

    return mouse_drag_motion;
};
