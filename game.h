#ifndef _GAME__H
#define _GAME__H

#include "graphics.h"
#include "knight.h"
#include "camera.h"
#include <SDL.h>

class Game {
private:
    SDL_Texture* bg = nullptr;
    Knight knight;
    Camera camera;

public:
    void init(Graphics& graphics) {
        bg = graphics.loadTexture(STAGE1_BACKGROUND_IMG);
        knight.init(graphics);
    }

    void handleEvent(SDL_Event& e) {
        knight.handleEvent(e);
    }

    void update() {
        knight.update(camera);
        camera.update(knight.getX(), knight.getWidth());
    }

    void render(Graphics& graphics) {
        graphics.prepareScene();
        graphics.renderTexture(bg, -camera.x, -camera.y, camera);
        knight.render(graphics, camera);
        graphics.presentScene();
    }

    void quit() {
        SDL_DestroyTexture(bg);
    }
};

#endif
