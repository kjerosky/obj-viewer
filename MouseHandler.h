#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include <SDL3/SDL.h>

#include "containers_3d.h"

class MouseHandler {

public:

    MouseHandler(SDL_Window* window);
    ~MouseHandler();

    void handle_left_button_status(bool is_button_down);
    Vector2 handle_mouse_motion(float relative_x, float relative_y);

private:

    SDL_Window* window;

    bool is_left_button_down;
};

#endif
