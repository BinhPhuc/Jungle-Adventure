#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "graphics.h"
#include "defs.h"
#include "menu.h"

using namespace std;

int main(int argc, char *argv[])
{
    Graphics graphics;
    graphics.init();

    SDL_Texture* background = graphics.loadTexture(STAGE1_BACKGROUND_IMG);

    Menu menu;
    menu.init(graphics);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        graphics.prepareScene(background);
//        menu.renderOverlay(graphics);
        menu.render(graphics);
        graphics.presentScene();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            MenuResult result = menu.handleEvent(e);
            if (result == MENU_START) {
                // TODO: chuyển sang chơi game
                quit = true;
            } else if (result == MENU_OPTION) {
                // TODO: hiện menu tùy chỉnh
            }
        }

        SDL_Delay(16);
    }

    menu.quit();
    SDL_DestroyTexture(background);
    graphics.quit();
    return 0;
}
