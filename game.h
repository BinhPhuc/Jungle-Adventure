#ifndef _GAME__H
#define _GAME__H

#include "graphics.h"
#include "knight.h"
#include "camera.h"
#include "tilemap.h"
#include <SDL.h>

class Game {
private:
    SDL_Texture* bg = nullptr;
    Knight knight;
    Camera camera;
    SDL_Texture* sky = nullptr;
    SDL_Texture* mountain = nullptr;

    TileMap tilemap;

public:
    void init(Graphics& graphics) {
        bg = graphics.loadTexture(STAGE1_BACKGROUND_IMG);
        knight.init(graphics);
        sky = graphics.loadTexture("assets/env/Sky.png");
        mountain = graphics.loadTexture("assets/env/BackGround.png");
        tilemap.load(graphics, "assets/env/TileSet.png", "assets/map/map.txt", TILE_SIZE, TILE_SIZE);
    }

    void handleEvent(SDL_Event& e) {
        knight.handleEvent(e);
    }

    void renderBackground(Graphics& graphics, SDL_Texture* bgTexture, const Camera& cam) {
        SDL_Rect src = { cam.x, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_Rect dest = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(graphics.renderer, bgTexture, &src, &dest);
    }

    void update() {
        int mapPixelWidth = tilemap.getCols() * TILE_SIZE;
        knight.update(camera, mapPixelWidth);
//        knight.update(camera);
        camera.update(knight.getX(), knight.getWidth());
    }

    void render(Graphics& graphics) {
        graphics.prepareScene();
        graphics.renderBackgroundLayer(sky, camera.x, 0.2f);
        graphics.renderBackgroundLayer(mountain, camera.x, 0.4f);

        tilemap.render(graphics, camera);
        knight.render(graphics, camera);
        graphics.presentScene();
    }

    void quit() {
        SDL_DestroyTexture(sky); sky = nullptr;
        SDL_DestroyTexture(mountain); mountain = nullptr;
        tilemap.quit();
        SDL_DestroyTexture(bg);
    }
};

#endif
