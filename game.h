// --- game.h ---
#ifndef _GAME__H
#define _GAME__H

#include "graphics.h"
#include "player.h"
#include "boss.h"
#include <vector>

enum GameState {
    CHOOSE_STAGE,
    IN_BATTLE
};

class Game {
private:
    GameState state = CHOOSE_STAGE;
    int selectedStage = 0;

    TTF_Font* font;
    SDL_Texture* battleBackground;
    Mix_Music* battleMusic;

    Player player;
    Boss boss;

    std::vector<std::string> stageNames = {"Boss 1", "Boss 2"};

public:
    void init(Graphics& graphics) {
        font = graphics.loadFont("assets/font/Coiny-Regular.ttf", 36);
        battleBackground = graphics.loadTexture("assets/imgs/bg.png");
        battleMusic = graphics.loadMusic("assets/music/battle_music.mp3");
    }

    void run(Graphics& graphics) {
        SDL_Event e;
        bool running = true;
        Mix_Pause(-1);
        graphics.play(battleMusic);
        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = false;
                if (state == CHOOSE_STAGE) {
                    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_2)) {
                        selectedStage = (e.key.keysym.sym == SDLK_1) ? 0 : 1;
                        enterBattle(graphics);
                        state = IN_BATTLE;
                    }
                } else if (state == IN_BATTLE) {
                    player.handleEvent(e);
                }
            }

            graphics.prepareScene(battleBackground);

            if (state == CHOOSE_STAGE) {
                renderStageSelection(graphics);
            } else if (state == IN_BATTLE) {
                updateBattle();
                renderBattle(graphics);
            }

            graphics.presentScene();
            SDL_Delay(16);
        }
    }

    void enterBattle(Graphics& graphics) {
        graphics.play(battleMusic);
        player.init(graphics);
        boss.init(graphics, 1000, 500, selectedStage);
    }

    void updateBattle() {
        player.update();
        boss.update();
    }

    void renderStageSelection(Graphics& graphics) {
        SDL_Color c = {255, 255, 255};
        SDL_Texture* txt1 = graphics.renderText("Press 1: Boss 1", font, c);
        SDL_Texture* txt2 = graphics.renderText("Press 2: Boss 2", font, c);

        graphics.renderTexture(txt1, SCREEN_WIDTH / 2 - 150, 250);
        graphics.renderTexture(txt2, SCREEN_WIDTH / 2 - 150, 320);

        SDL_DestroyTexture(txt1);
        SDL_DestroyTexture(txt2);
    }

    void renderBattle(Graphics& graphics) {
        player.render(graphics);
        boss.render(graphics);
    }
};

#endif
