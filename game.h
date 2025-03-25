#ifndef _GAME__H
#define _GAME__H

#include "graphics.h"
#include "player.h"
#include "warrior.h"
#include "boss.h"
#include "flyingdemon.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

enum GameState {
    CHOOSE_CHARACTER,
    PREVIEW_CHARACTER,
    CHOOSE_STAGE,
    PREVIEW_STAGE,
    IN_BATTLE,
    RETURN_TO_MENU,
    VICTORY,
    GAME_OVER
};

struct CharacterButton {
    std::string label;
    SDL_Rect rect;
    bool isHovered = false;
};

struct CharacterPreview {
    std::string characterName;
    std::string description;
    std::string characterImagePath;
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

struct Platform {
    SDL_Rect rect;
    SDL_Texture* texture = nullptr;
};

struct Chest {
    SDL_Rect rect;
    Sprite sprite;
    bool isOpened = false;
};

class Game {
private:
    GameState state = CHOOSE_CHARACTER;
    int selectedCharacter = -1;
    int selectedStage = -1;

    TTF_Font* font = nullptr;
    TTF_Font* titleFont = nullptr;
    TTF_Font* buttonFont = nullptr;
    SDL_Texture* currentBackground = nullptr;
    std::vector<SDL_Texture*> stageBackgrounds;
    Mix_Music* menuMusic = nullptr;
    Mix_Music* battleMusic = nullptr;
    Mix_Chunk* clickSound = nullptr;
    Mix_Chunk* errorClick = nullptr;

    std::vector<CharacterButton> characterButtons;
    std::vector<CharacterPreview> characterPreviews;
    std::vector<StageButton> stageButtons;
    std::vector<StagePreview> stagePreviews;

    SDL_Rect readyBtn = {900, 510, 150, 50};
    SDL_Rect backBtn = {900, 570, 150, 50};
    bool readyHovered = false;
    bool backHovered = false;

    Player* player = nullptr;
    Boss* boss = nullptr;
    std::vector<SDL_Rect> bossProjectiles;
    Uint32 resultDisplayTime = 0;

    SDL_Color normalTextColor = {255, 255, 255};
    SDL_Color hoverTextColor = {100, 255, 100};
    SDL_Color normalBoxColor = {0, 0, 0, 180};
    SDL_Color hoverBoxColor = {50, 50, 50, 220};

    // Chỉ giữ lại tên người chơi, bỏ ảnh
    std::string playerName = "Player"; // Tên người chơi mặc định

    std::vector<Platform> platforms; // Danh sách các thanh ngang
    bool bossDefeated = false; // Theo dõi xem boss đã bị hạ chưa

    Chest chest;

    int coins = 0;
    SDL_Rect buyHPBtn = {120, 20, 150, 50};  // Nút mua HP (bên cạnh chữ Coins)
    SDL_Rect buyATKBtn = {280, 20, 150, 50};// Nút mua ATK (xếp dọc dưới HP)
    bool buyHPHovered = false;
    bool buyATKHovered = false;

public:
    void init(Graphics& graphics) {
        font = graphics.loadFont("assets/font/Coiny-Regular.ttf", 24);
        buttonFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 32);
        titleFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 48);

        Platform platform1;
        platform1.rect = {400, 400, 154, 53}; // Vị trí và kích thước
        platform1.texture = graphics.loadTexture("assets/imgs/platform.png"); // Sprite cho platform
        platforms.push_back(platform1);

        Platform platform2;
        platform2.rect = {700, 300, 154, 53}; // Thanh thứ hai
        platform2.texture = graphics.loadTexture("assets/imgs/platform.png");
        platforms.push_back(platform2);

        chest.rect = {750, 250, 48, 48}; // Vị trí trên platform2
        SDL_Texture* chestTex = graphics.loadTexture("assets/sprites/chest.png");
        chest.sprite.initAuto(chestTex, 96, 96, 3); // 2 frame: đóng và mở
        chest.sprite.frameDelay = 100;

        std::ifstream configFile("config.txt");
        if (configFile.is_open()) {
            std::string line;
            // Bỏ qua dòng đầu tiên (âm thanh)
            std::getline(configFile, line);
            // Lấy dòng thứ hai (tên người chơi)
            if (std::getline(configFile, line)) {
                playerName = line;
            }
            configFile.close();
        } else {
            playerName = "Player"; // Mặc định nếu không đọc được file
        }

        // Tải background mặc định (dùng cho menu)
        currentBackground = graphics.loadTexture("assets/imgs/bg.png");

        // Tải background cho từng stage
        stageBackgrounds.push_back(graphics.loadTexture("assets/imgs/stage1.png")); // Stage 1
        stageBackgrounds.push_back(graphics.loadTexture("assets/imgs/stage2.png")); // Stage 2 (giả định)

        // Tải nhạc
        menuMusic = graphics.loadMusic("assets/music/menu_music.wav");
        battleMusic = graphics.loadMusic("assets/music/battle_music.mp3");
        clickSound = graphics.loadSound("assets/sounds/click.wav");
        errorClick = graphics.loadSound("assets/sounds/error.mp3");

        // Khởi tạo các nút chọn nhân vật
        characterButtons.push_back({"Warrior", {100, 250, 200, 80}});
        characterButtons.push_back({"Mage", {100, 350, 200, 80}});
        characterButtons.push_back({"Back", {100, 450, 200, 80}});

        // Khởi tạo thông tin preview cho từng nhân vật
        characterPreviews.push_back({"Warrior", "A brave warrior with a mighty sword.", "assets/characters/warrior.png"});
        characterPreviews.push_back({"Mage", "A powerful mage with magical spells.", "assets/characters/mage.png"});

        // Khởi tạo các nút chọn stage
        stageButtons.push_back({"Stage 1", {100, 250, 200, 80}});
        stageButtons.push_back({"Stage 2", {100, 350, 200, 80}});
        stageButtons.push_back({"Back", {100, 450, 200, 80}});

        // Khởi tạo thông tin preview cho từng stage
        stagePreviews.push_back({"Flying Demon", "A fierce flying demon lurking in the shadows.",
                                "assets/sprites/flying_demon/boss_flying_demon.png"});
        stagePreviews.push_back({"Mystery Creature", "A mysterious creature with unknown powers.", "assets/sprites/boss/boss2.png"});

        // Điều chỉnh vị trí của nút READY và BACK để nằm cạnh nhau
        int btnWidth = 150;
        int btnHeight = 50;
        int spacing = 20;
        int totalWidth = btnWidth * 2 + spacing;
        int startX = 800 + (400 - totalWidth) / 2;
        int btnY = 510;

        readyBtn = {startX, btnY, btnWidth, btnHeight};
        backBtn = {startX + btnWidth + spacing, btnY, btnWidth, btnHeight};

        // Phát nhạc menu (nếu có)
        if (menuMusic) {
            graphics.play(menuMusic);
        }

        coins = loadCoins();
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

            graphics.prepareScene(currentBackground);
            render(graphics);
            graphics.presentScene();
            SDL_Delay(16);
        }
        return RETURN_TO_MENU;
    }

private:
    void addCoins(int amount) {
        int totalCoins = loadCoins() + amount;
        std::ofstream out("coins.txt");
        if (out) {
            out << totalCoins << std::endl;
            out.close();
        }
        coins = totalCoins;
    }

    int loadCoins() {
        std::ifstream in("coins.txt");
        int coins = 0;
        if (in) {
            in >> coins;
            in.close();
        }
        return coins;
    }
    void renderShopUI(Graphics& graphics) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        buyHPHovered = inRect(mx, my, buyHPBtn);
        buyATKHovered = inRect(mx, my, buyATKBtn);

        // Hiển thị số xu ở (0, 0)
        std::string coinStr = "Coins: " + std::to_string(coins);
        SDL_Texture* coinTex = graphics.renderText(coinStr.c_str(), font, normalTextColor);
        if (coinTex) {
            int tw, th;
            SDL_QueryTexture(coinTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(coinTex, 0, 40); // Đặt ở (0, 0)
            SDL_DestroyTexture(coinTex);
        }

        // Nút mua HP ở (100, 0)
        SDL_Color hpColor = buyHPHovered ? hoverTextColor : normalTextColor;
        SDL_SetRenderDrawColor(graphics.renderer, normalBoxColor.r, normalBoxColor.g, normalBoxColor.b, normalBoxColor.a);
        SDL_RenderFillRect(graphics.renderer, &buyHPBtn);
        SDL_Texture* hpTex = graphics.renderText("HP (+10)", font, hpColor);
        if (hpTex) {
            int tw, th;
            SDL_QueryTexture(hpTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(hpTex, buyHPBtn.x + (buyHPBtn.w - tw) / 2, buyHPBtn.y + (buyHPBtn.h - th) / 2);
            SDL_DestroyTexture(hpTex);
        }

        // Nút mua ATK ở (260, 0)
        SDL_Color atkColor = buyATKHovered ? hoverTextColor : normalTextColor;
        SDL_SetRenderDrawColor(graphics.renderer, normalBoxColor.r, normalBoxColor.g, normalBoxColor.b, normalBoxColor.a);
        SDL_RenderFillRect(graphics.renderer, &buyATKBtn);
        SDL_Texture* atkTex = graphics.renderText("ATK (+3)", font, atkColor);
        if (atkTex) {
            int tw, th;
            SDL_QueryTexture(atkTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(atkTex, buyATKBtn.x + (buyATKBtn.w - tw) / 2, buyATKBtn.y + (buyATKBtn.h - th) / 2);
            SDL_DestroyTexture(atkTex);
        }
    }
    void fadeOut(Graphics& graphics) {
        SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        for (int alpha = 0; alpha <= 255; alpha += 5) {
            SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, alpha);
            SDL_RenderFillRect(graphics.renderer, &screenRect);
            SDL_RenderPresent(graphics.renderer);
            SDL_Delay(10);
        }
    }

    GameState handleEvent(SDL_Event& e, Graphics& graphics) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        if (state == CHOOSE_CHARACTER) {
            for (auto& btn : characterButtons) {
                btn.isHovered = inRect(mx, my, btn.rect);
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                SDL_Point mousePoint = {mx, my};
                if (SDL_PointInRect(&mousePoint, &characterButtons[0].rect)) {
                    graphics.play(clickSound);
                    selectedCharacter = 0;
                    state = PREVIEW_CHARACTER;
                } else if (SDL_PointInRect(&mousePoint, &characterButtons[1].rect)) {
                    graphics.play(clickSound);
                    selectedCharacter = 1;
                    state = PREVIEW_CHARACTER;
                } else if (SDL_PointInRect(&mousePoint, &characterButtons[2].rect)) {
                    graphics.play(clickSound);
                    fadeOut(graphics);
                    return RETURN_TO_MENU;
                }
            }
        } else if (state == PREVIEW_CHARACTER) {
            readyHovered = inRect(mx, my, readyBtn);
            backHovered = inRect(mx, my, backBtn);
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (readyHovered) {
                    graphics.play(clickSound);
                    state = CHOOSE_STAGE;
                } else if (backHovered) {
                    graphics.play(clickSound);

                    state = CHOOSE_CHARACTER;
                }
            }

        } else if (state == CHOOSE_STAGE) {
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
                    fadeOut(graphics);
                    state = CHOOSE_CHARACTER;
                }
            }
        } else if (state == PREVIEW_STAGE) {
            readyHovered = inRect(mx, my, readyBtn);
            backHovered = inRect(mx, my, backBtn);
            buyHPHovered = inRect(mx, my, buyHPBtn);
            buyATKHovered = inRect(mx, my, buyATKBtn);

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (readyHovered) {
                    graphics.play(clickSound);
                    enterBattle(graphics);
                    state = IN_BATTLE;
                } else if (backHovered) {
                    graphics.play(clickSound);
                    state = CHOOSE_STAGE;
                } else if (buyHPHovered && coins >= 10) {
                    coins -= 10;
                    Warrior* warrior = dynamic_cast<Warrior*>(player);
                    if (warrior) {
                        warrior->setHP(warrior->getHP() + 10); // Tăng HP
                    }
                    addCoins(-10);
                    graphics.play(clickSound);
                } else if (buyATKHovered && coins >= 10) {
                    coins -= 10;
                    Warrior* warrior = dynamic_cast<Warrior*>(player);
                    if (warrior) {
                        warrior->setAttackDamage(warrior->getAttackDamage() + 3); // Tăng ATK +3
                    }
                    addCoins(-10);
                    graphics.play(clickSound);
                } else if (buyHPHovered && coins < 10) {
                    graphics.play(errorClick);
                } else if (buyATKHovered && coins < 10) {
                    graphics.play(errorClick);
                }
            }
        } else if (state == IN_BATTLE) {
            player->handleEvent(e);
        } else if (state == VICTORY || state == GAME_OVER) {
            if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
                if (SDL_GetTicks() - resultDisplayTime >= 2000) {
                    fadeOut(graphics);
                    state = PREVIEW_STAGE;
                }
            }
        }
        return state;
    }

    void render(Graphics& graphics) {
        SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 100);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(graphics.renderer, &overlay);
        if (state == CHOOSE_CHARACTER || state == CHOOSE_STAGE) {
            SDL_Color titleColor = {255, 255, 0}; // Màu vàng
            std::string text = (state == CHOOSE_CHARACTER) ? "Choose Your Character" : "Choose Stage";
            SDL_Texture* titleTex = graphics.renderText(text.c_str(), titleFont, titleColor);
            if (titleTex) {
                int tw, th;
                SDL_QueryTexture(titleTex, NULL, NULL, &tw, &th);
                int tx = (SCREEN_WIDTH - tw) / 2; // Căn giữa theo chiều ngang
                int ty = 80; // Vị trí y = 80
                graphics.renderTexture(titleTex, tx, ty);
                SDL_DestroyTexture(titleTex);
            }
        }
        if (state == CHOOSE_CHARACTER || state == PREVIEW_CHARACTER) {
            renderCharacterButtons(graphics);
        }
        if (state == PREVIEW_CHARACTER) {
            renderCharacterPreview(graphics);
        } else if (state == CHOOSE_STAGE || state == PREVIEW_STAGE) {
            renderStageButtons(graphics);
        }
        if (state == PREVIEW_STAGE) {
            renderStagePreview(graphics);
        } else if (state == IN_BATTLE) {
            updateBattle(graphics);
            renderBattle(graphics);
        } else if (state == VICTORY) {
            renderResult(graphics, "Victory!");
        } else if (state == GAME_OVER) {
            renderResult(graphics, "Game Over!");
        }
    }

    void renderCharacterButtons(Graphics& graphics) {
        for (const auto& btn : characterButtons) {
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

    void renderCharacterPreview(Graphics& graphics) {
        const CharacterPreview& preview = characterPreviews[selectedCharacter];

        SDL_Rect previewBox = {800, 100, 400, 500};
        SDL_SetRenderDrawColor(graphics.renderer, 50, 50, 50, 220);
        SDL_RenderFillRect(graphics.renderer, &previewBox);

        SDL_Texture* characterNameTex = graphics.renderText(preview.characterName.c_str(), titleFont, normalTextColor);
        if (characterNameTex) {
            int tw, th;
            SDL_QueryTexture(characterNameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(characterNameTex, previewBox.x + (previewBox.w - tw) / 2, previewBox.y + 20);
            SDL_DestroyTexture(characterNameTex);
        }

        SDL_Texture* characterImg = graphics.loadTexture(preview.characterImagePath.c_str());
        if (characterImg) {
            int charW, charH;
            SDL_QueryTexture(characterImg, NULL, NULL, &charW, &charH);
            float scale = 0.6f;
            int scaledW = static_cast<int>(charW * scale);
            int scaledH = static_cast<int>(charH * scale);
            SDL_Rect charDst = {previewBox.x + (previewBox.w - scaledW) / 2, previewBox.y + 100, scaledW, scaledH};
            SDL_RenderCopy(graphics.renderer, characterImg, NULL, &charDst);
            SDL_DestroyTexture(characterImg);
        }

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

        SDL_Rect previewBox = {800, 100, 400, 500};
        SDL_SetRenderDrawColor(graphics.renderer, 50, 50, 50, 220);
        SDL_RenderFillRect(graphics.renderer, &previewBox);

        SDL_Texture* bossNameTex = graphics.renderText(preview.bossName.c_str(), titleFont, normalTextColor);
        if (bossNameTex) {
            int tw, th;
            SDL_QueryTexture(bossNameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(bossNameTex, previewBox.x + (previewBox.w - tw) / 2, previewBox.y + 20);
            SDL_DestroyTexture(bossNameTex);
        }

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
        renderShopUI(graphics);
    }

    void enterBattle(Graphics& graphics) {
        // Dừng nhạc menu nếu đang phát
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic();
        }

        // Phát nhạc chiến đấu
        if (battleMusic) {
            graphics.play(battleMusic);
        }

        // Cập nhật background dựa trên stage
        if (selectedStage >= 0 && selectedStage < static_cast<int>(stageBackgrounds.size())) {
            currentBackground = stageBackgrounds[selectedStage];
        }

        // Khởi tạo nhân vật
        if (selectedCharacter == 0) {
            player = new Warrior();
        } else {
            player = new Warrior(); // Chưa có Mage
        }
        player->init(graphics);

        // Khởi tạo Boss dựa trên stage
        if (boss) {
            delete boss;
            boss = nullptr;
        }
        if (selectedStage == 0) {
            boss = new FlyingDemon();
        } else {
            boss = new FlyingDemon(); // Chưa có Boss cho Stage 2, tạm dùng FlyingDemon
        }
        boss->init(graphics, 1000, 550, selectedStage);
        bossProjectiles.clear();
    }

    void updateBattle(Graphics& graphics) {
        player->update();
        boss->update();
        boss->attack(bossProjectiles);

        Warrior* warrior = dynamic_cast<Warrior*>(player);
        if (warrior) {
            float y = warrior->getY();
            float& jumpVelocity = warrior->getJumpVelocity(); // Cần thêm getter/setter trong warrior.h
            for (const auto& platform : platforms) {
                SDL_Rect playerRect = {static_cast<int>(warrior->getX()), static_cast<int>(y), 96, 84};
                if (SDL_HasIntersection(&playerRect, &platform.rect) && jumpVelocity > 0) {
                    y = platform.rect.y - 155; // Đặt nhân vật trên platform
                    warrior->setY(y);
                    jumpVelocity = 0;
                    warrior->getIsJumping() = false; // Cần thêm getter/setter trong warrior.h
                    warrior->setState(PlayerState::IDLE);
                    break;
                }
            }
        }

        // Cập nhật đạn của Boss
        for (auto it = bossProjectiles.begin(); it != bossProjectiles.end();) {
            it->x -= 5; // Giảm tốc độ đạn từ 5 xuống 3 pixel mỗi frame
            if (it->x < 0) {
                it = bossProjectiles.erase(it);
            } else {
                // Kiểm tra va chạm với Player
                SDL_Rect playerRect = {static_cast<int>(player->getX()), static_cast<int>(player->getY()), 96, 84};
                if (SDL_HasIntersection(&(*it), &playerRect)) {
                    player->takeDamage(boss->getAttackDamage());
                    it = bossProjectiles.erase(it);
                } else {
                    ++it;
                }
            }
        }

        // Kiểm tra va chạm cận chiến của Warrior với Boss
        static Uint32 lastAttackTime = 0; // Theo dõi thời gian lần tấn công cuối cùng
        if (player->getState() == PlayerState::ATTACK1) {
            float distance = std::abs(player->getX() - boss->getX());
            if (distance < 300) { // Tăng khoảng cách tấn công lên 300 pixel
                Uint32 currentTime = SDL_GetTicks();
                // Chỉ gây sát thương nếu đã qua 1 giây kể từ lần tấn công cuối
                if (currentTime - lastAttackTime >= 1000) { // 1000ms = 1 giây
                    boss->takeDamage(player->getAttackDamage());
                    lastAttackTime = currentTime;
                }
            }
        }

        // Khi boss chết, đánh dấu bossDefeated và bắt đầu đếm thời gian
        static Uint32 bossDefeatedTime = 0;
        if (boss->getHP() <= 0 && boss->getState() == BossState::DEATH && !bossDefeated) {
            bossDefeated = true;
            bossDefeatedTime = SDL_GetTicks(); // Ghi lại thời điểm boss chết
        }

        // Sau 5 giây kể từ khi boss chết, tự động mở rương và chuyển sang VICTORY
        if (bossDefeated && !chest.isOpened) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - bossDefeatedTime >= 5000) { // 5000ms = 5 giây
                chest.isOpened = true;
                chest.sprite.currentFrame = 2; // Chuyển sang frame mở
                addCoins(5); // Thêm 5 xu
                state = VICTORY;
                resultDisplayTime = SDL_GetTicks();
            }
        }

        // Kiểm tra thua
        if (player->getHP() <= 0 && player->getState() == PlayerState::DEATH) {
            state = GAME_OVER;
            resultDisplayTime = SDL_GetTicks();
        }
    }

    void renderBattle(Graphics& graphics) {
        for (const auto& platform : platforms) {
            graphics.renderTexture(platform.texture, platform.rect.x, platform.rect.y);
        }
        player->render(graphics);
        boss->render(graphics);
        if (bossDefeated) {
            graphics.renderSprite(chest.rect.x, chest.rect.y, chest.sprite, false, 1.0f); // Vẽ rương
        }
        // Vẽ đạn của Boss
        for (const auto& projectile : bossProjectiles) {
            boss->renderProjectile(graphics, projectile);
        }

        // Vẽ thanh máu và thanh nộ
        renderHealthBars(graphics);
    }

    void renderHealthBars(Graphics& graphics) {
        int baseY = 20; // Vị trí y cơ sở

        // Hiển thị tên người chơi
        SDL_Texture* nameTex = graphics.renderText(playerName.c_str(), font, normalTextColor);
        if (nameTex) {
            int tw, th;
            SDL_QueryTexture(nameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(nameTex, 20, baseY);
            SDL_DestroyTexture(nameTex);
        }

        // HP: Số HP + Thanh HP
        int healthBarY = baseY + 30; // Dịch xuống dưới tên
        std::string hpStr = "HP: " + std::to_string(player->getHP());
        SDL_Texture* hpTex = graphics.renderText(hpStr.c_str(), font, normalTextColor);
        int hpTextWidth = 0;
        if (hpTex) {
            int tw, th;
            SDL_QueryTexture(hpTex, NULL, NULL, &tw, &th);
            hpTextWidth = tw;
            graphics.renderTexture(hpTex, 20, healthBarY);
            SDL_DestroyTexture(hpTex);
        }
        SDL_Rect playerHealthBar = {20 + hpTextWidth + 30, healthBarY, player->getHP() * 2, 20};
        SDL_SetRenderDrawColor(graphics.renderer, 0, 255, 0, 255); // Màu xanh lá
        SDL_RenderFillRect(graphics.renderer, &playerHealthBar);

        // ATK: Số ATK
        int atkY = healthBarY + 30;
        std::string atkStr = "ATK: " + std::to_string(player->getAttackDamage());
        SDL_Texture* atkTex = graphics.renderText(atkStr.c_str(), font, normalTextColor);
        if (atkTex) {
            int tw, th;
            SDL_QueryTexture(atkTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(atkTex, 20, atkY);
            SDL_DestroyTexture(atkTex);
        }

        // Rage: Số Rage + Thanh nộ
        Warrior* warrior = dynamic_cast<Warrior*>(player);
        int rageY = atkY + 30;
        if (warrior) {
            std::string rageStr = "Rage: " + std::to_string(static_cast<int>(warrior->getRage())) + "%";
            SDL_Texture* rageTex = graphics.renderText(rageStr.c_str(), font, normalTextColor);
            int rageTextWidth = 0;
            if (rageTex) {
                int tw, th;
                SDL_QueryTexture(rageTex, NULL, NULL, &tw, &th);
                rageTextWidth = tw;
                graphics.renderTexture(rageTex, 20, rageY);
                SDL_DestroyTexture(rageTex);
            }
            SDL_Rect rageBar = {20 + rageTextWidth + 30, rageY, static_cast<int>(warrior->getRage() * 2), 10};
            SDL_SetRenderDrawColor(graphics.renderer, 255, 165, 0, 255); // Màu cam
            SDL_RenderFillRect(graphics.renderer, &rageBar);
        }

        // Thanh máu và thông tin của Boss
        int bossBaseY = 20; // Vị trí y cơ sở cho Boss

        // Hiển thị tên Boss
        const StagePreview& preview = stagePreviews[selectedStage];
        SDL_Texture* bossNameTex = graphics.renderText(preview.bossName.c_str(), font, normalTextColor);
        if (bossNameTex) {
            int tw, th;
            SDL_QueryTexture(bossNameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(bossNameTex, SCREEN_WIDTH - 20 - tw, bossBaseY);
            SDL_DestroyTexture(bossNameTex);
        }

        // HP: Số HP + Thanh HP của Boss
        int bossHealthBarY = bossBaseY + 30;
        std::string bossHpStr = "HP: " + std::to_string(boss->getHP());
        SDL_Texture* bossHpTex = graphics.renderText(bossHpStr.c_str(), font, normalTextColor);
        int bossHpTextWidth = 0;
        if (bossHpTex) {
            int tw, th;
            SDL_QueryTexture(bossHpTex, NULL, NULL, &tw, &th);
            bossHpTextWidth = tw;
            graphics.renderTexture(bossHpTex, SCREEN_WIDTH - 20 - bossHpTextWidth, bossHealthBarY);
            SDL_DestroyTexture(bossHpTex);
        }
        SDL_Rect bossHealthBar = {SCREEN_WIDTH - 20 - bossHpTextWidth - boss->getHP() * 2 - 10, bossHealthBarY, boss->getHP() * 2, 20};
        SDL_SetRenderDrawColor(graphics.renderer, 255, 0, 0, 255); // Màu đỏ
        SDL_RenderFillRect(graphics.renderer, &bossHealthBar);

        // ATK: Số ATK của Boss
        int bossAtkY = bossHealthBarY + 30;
        std::string bossAtkStr = "ATK: " + std::to_string(boss->getAttackDamage());
        SDL_Texture* bossAtkTex = graphics.renderText(bossAtkStr.c_str(), font, normalTextColor);
        if (bossAtkTex) {
            int tw, th;
            SDL_QueryTexture(bossAtkTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(bossAtkTex, SCREEN_WIDTH - 20 - tw, bossAtkY);
            SDL_DestroyTexture(bossAtkTex);
        }
    }

    void renderResult(Graphics& graphics, const std::string& message) {
        SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 200);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(graphics.renderer, &overlay);

        SDL_Texture* resultTex = graphics.renderText(message.c_str(), titleFont, normalTextColor);
        if (resultTex) {
            int tw, th;
            SDL_QueryTexture(resultTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(resultTex, (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT - th) / 2);
            SDL_DestroyTexture(resultTex);
        }
    }

    bool inRect(int x, int y, SDL_Rect rect) {
        return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
    }

public:
    void quit() {
        if (player) {
            delete player;
            player = nullptr;
        }
        if (boss) {
            delete boss;
            boss = nullptr;
        }
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        if (titleFont) {
            TTF_CloseFont(titleFont);
            titleFont = nullptr;
        }
        if (buttonFont) {
            TTF_CloseFont(buttonFont);
            buttonFont = nullptr;
        }
        if (currentBackground) {
            SDL_DestroyTexture(currentBackground);
            currentBackground = nullptr;
        }
        for (auto& bg : stageBackgrounds) {
            if (bg) {
                SDL_DestroyTexture(bg);
                bg = nullptr;
            }
        }
        stageBackgrounds.clear();
        if (menuMusic) {
            Mix_FreeMusic(menuMusic);
            menuMusic = nullptr;
        }
        if (battleMusic) {
            Mix_FreeMusic(battleMusic);
            battleMusic = nullptr;
        }
        if (clickSound) {
            Mix_FreeChunk(clickSound);
            clickSound = nullptr;
        }
        for (auto& platform : platforms) {
            if (platform.texture) {
                SDL_DestroyTexture(platform.texture);
                platform.texture = nullptr;
            }
        }
    }
};

#endif
