#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "graphics.h"
#include "defs.h"
#include "menu.h"
#include "option.h"
#include "game.h"

using namespace std;

void fadeOut(SDL_Renderer* renderer) {
    SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    for (int alpha = 0; alpha <= 255; alpha += 5) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &screenRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
}

void fadeIn(SDL_Renderer* renderer) {
    SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    for (int alpha = 255; alpha >= 0; alpha -= 5) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &screenRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
}

int main(int argc, char *argv[])
{
    Graphics graphics;
    graphics.init();

    SDL_Texture* background = graphics.loadTexture(STAGE1_BACKGROUND_IMG);
    Mix_Chunk* clickSound = graphics.loadSound("assets/sounds/click.wav");
    Mix_Music* menuMusic = graphics.loadMusic("assets/music/background_music.mp3");

    int volume;
    std::ifstream in("config.txt");
    if (in) {
        in >> volume;
        if (volume != 0) {
            graphics.play(menuMusic);
        }
        in.ignore();
        in.close();
    }

    Menu menu;
    menu.init(graphics);

    bool quit = false;
    SDL_Event e;

    ScrollingBackground scrollBackground;
    scrollBackground.setTexture(background);

    while (!quit) {
        graphics.prepareScene(background);
        menu.renderOverlay(graphics);
        menu.render(graphics);
        graphics.presentScene();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            MenuResult result = menu.handleEvent(e, graphics, clickSound);

            if (result == MENU_START) {
                graphics.play(clickSound);
                fadeOut(graphics.renderer);

                Game game;
                game.init(graphics);

                SDL_Event gameEvent;
                bool gameRunning = true;
                while (gameRunning) {
                    while (SDL_PollEvent(&gameEvent)) {
                        if (gameEvent.type == SDL_QUIT) {
                            gameRunning = false;
                            quit = true;
                        }
                        game.handleEvent(gameEvent);
                    }

                    game.update();
                    game.render(graphics);

                    SDL_Delay(16); // ~60 FPS
                }

                game.quit();
                fadeIn(graphics.renderer);
            } else if (result == MENU_OPTION) {
                OptionMenu option;
                option.init(graphics);
                bool optionRunning = true;
                bool backPressed = false;
                while (optionRunning) {
                    while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_QUIT) {
                            optionRunning = false;
                            quit = true;
                        }
                        OptionResult opt = option.handleEvent(e, graphics, clickSound);
                        if (opt == OPTION_BACK) {
                            optionRunning = false;
                            backPressed = true;
                        }
                    }
                    graphics.prepareScene(background);
                    option.render(graphics);
                    graphics.presentScene();

                    SDL_Delay(16);
                }
                option.quit();
            }
        }

        SDL_Delay(16);
    }

    menu.quit();
    SDL_DestroyTexture(background);
    graphics.quit();
    return 0;
}
