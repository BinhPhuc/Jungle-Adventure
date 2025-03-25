#ifndef _GAME__H
#define _GAME__H

#include "graphics.h"
#include "player.h"
#include "boss.h"
#include <vector>

enum GameState {
    CHOOSE_STAGE,
    PREVIEW_STAGE,
    IN_BATTLE
};

class Game {
private:
    GameState state = CHOOSE_STAGE;
    int selectedStage = -1;

    TTF_Font* font;
    SDL_Texture* battleBackground;
    Mix_Music* battleMusic;

    SDL_Texture* bossPreviewImg;
    SDL_Texture* bossDescriptionTxt;
    SDL_Rect stage1Btn = {100, 250, 200, 80};
    SDL_Rect stage2Btn = {100, 350, 200, 80};
    SDL_Rect readyBtn  = {900, 500, 200, 60};

    Player player;
    Boss boss;

public:
    void init(Graphics& graphics) {
        font = graphics.loadFont("assets/font/Coiny-Regular.ttf", 36);
        battleBackground = graphics.loadTexture("assets/imgs/bg.png");
        battleMusic = graphics.loadMusic("assets/music/battle_music.mp3");
    }

    void run(Graphics& graphics) {
        SDL_Event e;
        bool running = true;

        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = false;

                if (state == CHOOSE_STAGE) {
                    if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mx = e.button.x;
                        int my = e.button.y;
                        SDL_Point mousePoint = {mx, my};
                        if (SDL_PointInRect(&mousePoint, &stage1Btn)) {
                            selectedStage = 0;
                            loadStagePreview(graphics);
                            state = PREVIEW_STAGE;
                        } else if (SDL_PointInRect(&mousePoint, &stage2Btn)) {
                            selectedStage = 1;
                            loadStagePreview(graphics);
                            state = PREVIEW_STAGE;
                        }
                    }
                } else if (state == PREVIEW_STAGE) {
                    if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mx = e.button.x;
                        int my = e.button.y;
                        SDL_Point mousePoint = {mx, my};
                        if (SDL_PointInRect(&mousePoint, &readyBtn)) {
                            enterBattle(graphics);
                            state = IN_BATTLE;
                        }
                    }
                } else if (state == IN_BATTLE) {
                    player.handleEvent(e);
                }
            }

            graphics.prepareScene(battleBackground);

            if (state == CHOOSE_STAGE) {
                renderStageButtons(graphics);
            } else if (state == PREVIEW_STAGE) {
                renderStagePreview(graphics);
            } else if (state == IN_BATTLE) {
                updateBattle();
                renderBattle(graphics);
            }

            graphics.presentScene();
            SDL_Delay(16);
        }
    }

    void renderStageButtons(Graphics& graphics) {
        SDL_Color c = {255, 255, 255};
        SDL_Texture* t1 = graphics.renderText("Stage 1", font, c);
        SDL_Texture* t2 = graphics.renderText("Stage 2", font, c);

        graphics.renderTexture(t1, stage1Btn.x, stage1Btn.y);
        graphics.renderTexture(t2, stage2Btn.x, stage2Btn.y);

        SDL_DestroyTexture(t1);
        SDL_DestroyTexture(t2);
    }

    void loadStagePreview(Graphics& graphics) {
        SDL_Color white = {255, 255, 255};
        if (selectedStage == 0) {
            bossPreviewImg = graphics.loadTexture("assets/boss/boss_flying_demon.png");
            bossDescriptionTxt = graphics.renderText("A fierce flying demon. Lorem ipsum dolor sit amet.", font, white);
        } else {
            bossPreviewImg = graphics.loadTexture("assets/boss/boss2.png");
            bossDescriptionTxt = graphics.renderText("A mysterious creature. Lorem ipsum dolor sit amet.", font, white);
        }
    }

    void renderStagePreview(Graphics& graphics) {
        SDL_Color white = {255, 255, 255};

        SDL_Rect previewBox = {800, 100, 400, 500};
        SDL_SetRenderDrawColor(graphics.renderer, 50, 50, 50, 220);
        SDL_RenderFillRect(graphics.renderer, &previewBox);

        SDL_Texture* title = graphics.renderText("Flying Demon", graphics.loadFont("assets/font/Coiny-Regular.ttf", 48), white);
        graphics.renderTexture(title, previewBox.x + 30, previewBox.y + 20);
        SDL_DestroyTexture(title);

        SDL_Texture* desc = graphics.renderText("A fierce flying demon.", graphics.loadFont("assets/font/Coiny-Regular.ttf", 20), white);
        graphics.renderTexture(desc, previewBox.x + 30, previewBox.y + 90);
        SDL_DestroyTexture(desc);

        SDL_Texture* bossImg = graphics.loadTexture("assets/boss/boss_flying_demon.png");
        int bossW, bossH;
        SDL_QueryTexture(bossImg, NULL, NULL, &bossW, &bossH);

        float scale = 0.6f;
        int scaledW = static_cast<int>(bossW * scale);
        int scaledH = static_cast<int>(bossH * scale);

        SDL_Rect bossDst = {
            previewBox.x + (previewBox.w - scaledW) / 2,
            previewBox.y + 150,
            scaledW,
            scaledH
        };
        SDL_RenderCopy(graphics.renderer, bossImg, NULL, &bossDst);
        SDL_DestroyTexture(bossImg);

        SDL_Texture* ready = graphics.renderText("READY", graphics.loadFont("assets/font/Coiny-Regular.ttf", 32), white);
        graphics.renderTexture(ready, previewBox.x + 130, previewBox.y + 430);
        SDL_DestroyTexture(ready);
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

    void renderBattle(Graphics& graphics) {
        player.render(graphics);
        boss.render(graphics);
    }
};

#endif
