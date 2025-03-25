#ifndef _GAME__H
#define _GAME__H

#include "graphics.h"
#include "player.h"
#include "boss.h"
#include <vector>
#include <string>
#include <sstream>

enum GameState {
    CHOOSE_STAGE,
    PREVIEW_STAGE,
    IN_BATTLE,
    RETURN_TO_MENU
};

struct StageButton {
    std::string label;
    SDL_Rect rect;
    bool isHovered = false;
};

struct StagePreview {
    std::string bossName;
    std::string description;
    std::string bossImagePath;
};

class Game {
private:
    GameState state = CHOOSE_STAGE;
    int selectedStage = -1;

    TTF_Font* font = nullptr;
    TTF_Font* titleFont = nullptr;
    TTF_Font* buttonFont = nullptr;
    SDL_Texture* battleBackground = nullptr;
    Mix_Music* battleMusic = nullptr;
    Mix_Chunk* clickSound = nullptr;

    std::vector<StageButton> stageButtons;
    std::vector<StagePreview> stagePreviews;
    SDL_Rect readyBtn = {900, 510, 150, 50};
    SDL_Rect backBtn = {900, 570, 150, 50};
    bool readyHovered = false;
    bool backHovered = false;

    Player player;
    Boss boss;

    SDL_Color normalTextColor = {255, 255, 255};
    SDL_Color hoverTextColor = {100, 255, 100};
    SDL_Color normalBoxColor = {0, 0, 0, 180};
    SDL_Color hoverBoxColor = {50, 50, 50, 220};

public:
    void init(Graphics& graphics) {
        font = graphics.loadFont("assets/font/Coiny-Regular.ttf", 24);
        buttonFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 32);
        titleFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 48);
        battleBackground = graphics.loadTexture("assets/imgs/bg.png");
        battleMusic = graphics.loadMusic("assets/music/battle_music.mp3");
        clickSound = graphics.loadSound("assets/sounds/click.wav");

        // Khởi tạo các nút chọn stage
        stageButtons.push_back({"Stage 1", {100, 250, 200, 80}});
        stageButtons.push_back({"Stage 2", {100, 350, 200, 80}});
        stageButtons.push_back({"Back", {100, 450, 200, 80}});

        // Khởi tạo thông tin preview cho từng stage
        stagePreviews.push_back({"Flying Demon", "A fierce flying demon lurking in the shadows.", "assets/boss/boss_flying_demon.png"});
        stagePreviews.push_back({"Mystery Creature", "A mysterious creature with unknown powers.", "assets/boss/boss2.png"});

        // Điều chỉnh vị trí của nút READY và BACK để nằm cạnh nhau
        int btnWidth = 150;
        int btnHeight = 50;
        int spacing = 20;
        int totalWidth = btnWidth * 2 + spacing;
        int startX = 800 + (400 - totalWidth) / 2;
        int btnY = 510;

        readyBtn = {startX, btnY, btnWidth, btnHeight};
        backBtn = {startX + btnWidth + spacing, btnY, btnWidth, btnHeight};
    }

    GameState run(Graphics& graphics) {
        SDL_Event e;
        bool running = true;

        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    running = false;
                    return RETURN_TO_MENU;
                }
                GameState result = handleEvent(e, graphics);
                if (result == RETURN_TO_MENU) {
                    running = false;
                    return RETURN_TO_MENU;
                }
            }

            graphics.prepareScene(battleBackground);
            render(graphics);
            graphics.presentScene();
            SDL_Delay(16);
        }
        return RETURN_TO_MENU;
    }

private:
    GameState handleEvent(SDL_Event& e, Graphics& graphics) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        if (state == CHOOSE_STAGE || state == PREVIEW_STAGE) {
            for (auto& btn : stageButtons) {
                btn.isHovered = inRect(mx, my, btn.rect);
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                SDL_Point mousePoint = {mx, my};
                if (SDL_PointInRect(&mousePoint, &stageButtons[0].rect)) {
                    graphics.play(clickSound);
                    selectedStage = 0;
                    state = PREVIEW_STAGE;
                } else if (SDL_PointInRect(&mousePoint, &stageButtons[1].rect)) {
                    graphics.play(clickSound);
                    selectedStage = 1;
                    state = PREVIEW_STAGE;
                } else if (SDL_PointInRect(&mousePoint, &stageButtons[2].rect)) {
                    graphics.play(clickSound);
                    return RETURN_TO_MENU;
                }
            }
        }

        if (state == PREVIEW_STAGE) {
            readyHovered = inRect(mx, my, readyBtn);
            backHovered = inRect(mx, my, backBtn);
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (readyHovered) {
                    graphics.play(clickSound);
                    enterBattle(graphics);
                    state = IN_BATTLE;
                } else if (backHovered) {
                    graphics.play(clickSound);
                    state = CHOOSE_STAGE;
                }
            }
        } else if (state == IN_BATTLE) {
            player.handleEvent(e);
        }
        return state;
    }

    void render(Graphics& graphics) {
        SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 100);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(graphics.renderer, &overlay);

        // Chỉ hiển thị các nút stage trong trạng thái CHOOSE_STAGE và PREVIEW_STAGE
        if (state == CHOOSE_STAGE || state == PREVIEW_STAGE) {
            renderStageButtons(graphics);
        }

        if (state == PREVIEW_STAGE) {
            renderStagePreview(graphics);
        } else if (state == IN_BATTLE) {
            updateBattle();
            renderBattle(graphics);
        }
    }

    void renderStageButtons(Graphics& graphics) {
        for (const auto& btn : stageButtons) {
            SDL_Color boxColor = btn.isHovered ? hoverBoxColor : normalBoxColor;
            SDL_Color textColor = btn.isHovered ? hoverTextColor : normalTextColor;

            SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(graphics.renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
            SDL_RenderFillRect(graphics.renderer, &btn.rect);

            SDL_Texture* textTex = graphics.renderText(btn.label.c_str(), buttonFont, textColor);
            if (textTex) {
                int tw, th;
                SDL_QueryTexture(textTex, NULL, NULL, &tw, &th);
                graphics.renderTexture(textTex, btn.rect.x + (btn.rect.w - tw) / 2, btn.rect.y + (btn.rect.h - th) / 2);
                SDL_DestroyTexture(textTex);
            }
        }
    }

    // Hàm hỗ trợ để chia văn bản thành nhiều dòng
    std::vector<std::string> wrapText(const std::string& text, TTF_Font* font, int maxWidth) {
        std::vector<std::string> lines;
        std::stringstream ss(text);
        std::string word;
        std::string currentLine;

        while (ss >> word) {
            std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
            SDL_Surface* surface = TTF_RenderText_Solid(font, testLine.c_str(), normalTextColor);
            if (surface) {
                int textWidth = surface->w;
                SDL_FreeSurface(surface);

                if (textWidth <= maxWidth) {
                    currentLine = testLine;
                } else {
                    if (!currentLine.empty()) {
                        lines.push_back(currentLine);
                    }
                    currentLine = word;
                }
            }
        }

        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }

        return lines;
    }

    void renderStagePreview(Graphics& graphics) {
        const StagePreview& preview = stagePreviews[selectedStage];

        // Vẽ khung preview
        SDL_Rect previewBox = {800, 100, 400, 500};
        SDL_SetRenderDrawColor(graphics.renderer, 50, 50, 50, 220);
        SDL_RenderFillRect(graphics.renderer, &previewBox);

        // Vẽ tên boss
        SDL_Texture* bossNameTex = graphics.renderText(preview.bossName.c_str(), titleFont, normalTextColor);
        if (bossNameTex) {
            int tw, th;
            SDL_QueryTexture(bossNameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(bossNameTex, previewBox.x + (previewBox.w - tw) / 2, previewBox.y + 20);
            SDL_DestroyTexture(bossNameTex);
        }

        // Vẽ hình ảnh boss
        SDL_Texture* bossImg = graphics.loadTexture(preview.bossImagePath.c_str());
        if (bossImg) {
            int bossW, bossH;
            SDL_QueryTexture(bossImg, NULL, NULL, &bossW, &bossH);
            float scale = 0.6f;
            int scaledW = static_cast<int>(bossW * scale);
            int scaledH = static_cast<int>(bossH * scale);
            SDL_Rect bossDst = {previewBox.x + (previewBox.w - scaledW) / 2, previewBox.y + 100, scaledW, scaledH};
            SDL_RenderCopy(graphics.renderer, bossImg, NULL, &bossDst);
            SDL_DestroyTexture(bossImg);
        }

        // Vẽ mô tả (chia thành nhiều dòng nếu cần)
        std::vector<std::string> lines = wrapText(preview.description, font, previewBox.w - 40);
        int lineHeight = 30;
        int startY = previewBox.y + 350;
        for (size_t i = 0; i < lines.size(); ++i) {
            SDL_Texture* descTex = graphics.renderText(lines[i].c_str(), font, normalTextColor);
            if (descTex) {
                int tw, th;
                SDL_QueryTexture(descTex, NULL, NULL, &tw, &th);
                graphics.renderTexture(descTex, previewBox.x + (previewBox.w - tw) / 2, startY + i * lineHeight);
                SDL_DestroyTexture(descTex);
            }
        }

        SDL_Color readyColor = readyHovered ? hoverTextColor : normalTextColor;
        SDL_SetRenderDrawColor(graphics.renderer, normalBoxColor.r, normalBoxColor.g, normalBoxColor.b, normalBoxColor.a);
        SDL_RenderFillRect(graphics.renderer, &readyBtn);
        SDL_Texture* readyTex = graphics.renderText("READY", font, readyColor);
        if (readyTex) {
            int tw, th;
            SDL_QueryTexture(readyTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(readyTex, readyBtn.x + (readyBtn.w - tw) / 2, readyBtn.y + (readyBtn.h - th) / 2);
            SDL_DestroyTexture(readyTex);
        }

        SDL_Color backColor = backHovered ? hoverTextColor : normalTextColor;
        SDL_SetRenderDrawColor(graphics.renderer, normalBoxColor.r, normalBoxColor.g, normalBoxColor.b, normalBoxColor.a);
        SDL_RenderFillRect(graphics.renderer, &backBtn);
        SDL_Texture* backTex = graphics.renderText("BACK", font, backColor);
        if (backTex) {
            int tw, th;
            SDL_QueryTexture(backTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(backTex, backBtn.x + (backBtn.w - tw) / 2, backBtn.y + (backBtn.h - th) / 2);
            SDL_DestroyTexture(backTex);
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

    void renderBattle(Graphics& graphics) {
        player.render(graphics);
        boss.render(graphics);
    }

    bool inRect(int x, int y, SDL_Rect rect) {
        return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
    }

public:
    void quit() {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        if (titleFont) {
            TTF_CloseFont(titleFont);
            titleFont = nullptr;
        }
        if (battleBackground) {
            SDL_DestroyTexture(battleBackground);
            battleBackground = nullptr;
        }
        if (battleMusic) {
            Mix_FreeMusic(battleMusic);
            battleMusic = nullptr;
        }
        if (clickSound) {
            Mix_FreeChunk(clickSound);
            clickSound = nullptr;
        }
    }
};

#endif
