#ifndef _CONTROLS__H
#define _CONTROLS__H

#include <SDL.h>

struct ControlKey {
    SDL_Scancode key;
    std::string action;
    SDL_Texture* keyTexture;
};

#endif
