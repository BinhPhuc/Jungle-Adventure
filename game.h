#ifndef _GAME__H
#define _GAME__H

#include "graphics.h"
#include "player.h"
#include "warrior.h"
#include "boss.h"
#include "flyingdemon.h"
#include "demonslime.h"
#include "asteroid.h"
#include "shopitem.h"
#include "archer.h"

#include <algorithm>
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
    PAUSED,
    RETURN_TO_MENU,
    VICTORY,
    GAME_OVER,
    SHOP
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

struct ControlKey {
    SDL_Scancode key;
    std::string action;
    SDL_Texture* keyTexture;
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
    Mix_Chunk* diedMusic = nullptr;
    Mix_Chunk* victorySound = nullptr;

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
    std::vector<Asteroid> asteroids;
    bool screenShake;
    Uint32 screenShakeStartTime;
    int screenShakeDuration;
    int screenShakeMagnitude;
    int shakeOffsetX, shakeOffsetY;
    Uint32 resultDisplayTime = 0;

    SDL_Color normalTextColor = {255, 255, 255};
    SDL_Color hoverTextColor = {100, 255, 100};
    SDL_Color normalBoxColor = {0, 0, 0, 180};
    SDL_Color hoverBoxColor = {50, 50, 50, 220};

    std::string playerName = "Player";

    std::vector<Platform> platforms;
    bool bossDefeated = false;
    Chest chest;

    int coins = 0;
    SDL_Texture* shopBackground = nullptr;
    Sprite coinIcon;
    std::vector<ShopItem> shopItems;
    SDL_Rect startBattleButton;
    SDL_Rect backShopButton;
    bool startBattleButtonHovered;
    bool backShopButtonHovered;
    std::string shopMessage;
    Uint32 shopMessageTimer;

    // Biến cho menu pause
    SDL_Rect btnReturnToMenu;
    SDL_Rect sliderBar, sliderKnob;
    bool returnToMenuHovered = false;
    bool draggingSlider = false;
    int volume = 50;
    SDL_Color textColor = {255, 255, 255};
    SDL_Color highlightColor = {100, 255, 100};

    Uint32 bossDefeatedTime = 0;

    Uint32 playerDeathTime = 0;
    bool playerDead = false;

    SDL_Rect btnRetry;
    SDL_Rect btnReturnToMenuGameOver;
    bool retryHovered = false;
    bool returnToMenuGameOverHovered = false;

    SDL_Rect btnBack;
    bool backPauseHovered = false;

    bool playerInitialized = false;


    bool showControls = false;
    Uint32 controlsDisplayTime = 0;
    const Uint32 controlsDuration = 5000;

    std::vector<ControlKey> controls;
public:

    void renderControls(Graphics& graphics) {
        SDL_Rect overlay;
        if (state == PREVIEW_CHARACTER) {
            overlay = {350, 250, 400, 300};
        } else if (state == IN_BATTLE) {
            overlay = {(SCREEN_WIDTH - 400) / 2, (SCREEN_HEIGHT - 300) / 2, 400, 300};
        }
        SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 150);
        SDL_RenderFillRect(graphics.renderer, &overlay);

        int currentY = overlay.y + 20;
        SDL_Color white = {255, 255, 255, 255};
        for (const auto& control : controls) {
            SDL_Rect keyRect = {overlay.x + 20, currentY, 32, 32};
            SDL_RenderCopy(graphics.renderer, control.keyTexture, nullptr, &keyRect);

            SDL_Texture* actionText = graphics.renderText(control.action.c_str(), font, white);
            if (actionText) {
                int textW, textH;
                SDL_QueryTexture(actionText, nullptr, nullptr, &textW, &textH);
                SDL_Rect textRect = {overlay.x + 60, currentY + (32 - textH) / 2, textW, textH};
                SDL_RenderCopy(graphics.renderer, actionText, nullptr, &textRect);
                SDL_DestroyTexture(actionText);
            }
            currentY += 40;
        }

        if (state == IN_BATTLE) {
            std::string hint = "Press H to toggle";
            SDL_Texture* hintTexture = graphics.renderText(hint.c_str(), font, white);
            if (hintTexture) {
                int tw, th;
                SDL_QueryTexture(hintTexture, nullptr, nullptr, &tw, &th);
                SDL_Rect hintDst = {
                    overlay.x + (overlay.w - tw) / 2,
                    overlay.y + overlay.h - th - 10,
                    tw, th
                };
                SDL_RenderCopy(graphics.renderer, hintTexture, nullptr, &hintDst);
                SDL_DestroyTexture(hintTexture);
            }
        }
    }

    void initBackgrounds(Graphics& graphics) {
        stageBackgrounds.clear();
        stageBackgrounds.push_back(graphics.loadTexture("assets/imgs/stage1.png"));
        stageBackgrounds.push_back(graphics.loadTexture("assets/imgs/stage2.png"));
    }
    void init(Graphics& graphics) {
        font = graphics.loadFont("assets/font/Coiny-Regular.ttf", 24);
        buttonFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 32);
        titleFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 48);

        loadControls(graphics);

        Platform platform1;
        platform1.rect = {400, 400, 154, 53};
        platform1.texture = graphics.loadTexture("assets/imgs/platform.png");
        platforms.push_back(platform1);

        Platform platform2;
        platform2.rect = {700, 300, 154, 53};
        platform2.texture = graphics.loadTexture("assets/imgs/platform.png");
        platforms.push_back(platform2);

        chest.rect = {750, 250, 48, 48};
        SDL_Texture* chestTex = graphics.loadTexture("assets/sprites/chest.png");
        chest.sprite.initAuto(chestTex, 96, 96, 3);
        chest.sprite.frameDelay = 100;

        std::ifstream configFile("assets/database/config.txt");
        if (configFile.is_open()) {
            std::string line;
            if (std::getline(configFile, line)) {
                try {
                    volume = std::stoi(line);
                    Mix_VolumeMusic(volume * 128 / 100);
                } catch (...) {
                    volume = 50;
                    Mix_VolumeMusic(volume * 128 / 100);
                }
            }
            if (std::getline(configFile, line)) {
                playerName = line;
            }
            configFile.close();
        } else {
            playerName = "Player";
            volume = 50;
            Mix_VolumeMusic(volume * 128 / 100);
        }
        updateSliderKnob();

        currentBackground = graphics.loadTexture("assets/imgs/bg.png");
        initBackgrounds(graphics);

        menuMusic = graphics.loadMusic("assets/music/menu_music.wav");
        battleMusic = graphics.loadMusic("assets/music/battle_music.mp3");
        clickSound = graphics.loadSound("assets/sounds/click.wav");
        errorClick = graphics.loadSound("assets/sounds/error.mp3");
        diedMusic = graphics.loadSound("assets/sounds/you_died.mp3");
        victorySound = graphics.loadSound("assets/sounds/victory.wav");

        characterButtons.push_back({"Warrior", {100, 250, 200, 80}});
        characterButtons.push_back({"Archer", {100, 350, 200, 80}});
        characterButtons.push_back({"Back", {100, 450, 200, 80}});

        characterPreviews.push_back({"Warrior", "A brave warrior with a mighty sword.", "assets/characters/warrior.png"});
        characterPreviews.push_back({"Archer", "A skilled archer with deadly arrows.", "assets/characters/archer.png"});

        stageButtons.push_back({"Stage 1", {100, 250, 200, 80}});
        stageButtons.push_back({"Stage 2", {100, 350, 200, 80}});
        stageButtons.push_back({"Back", {100, 450, 200, 80}});

        stagePreviews.push_back({"Flying Demon", "A fierce flying demon lurking in the shadows.",
                                "assets/sprites/flying_demon/boss_flying_demon.png"});
        stagePreviews.push_back({"Demon Slime", "A fiery demon slime wielding a deadly sword.", "assets/sprites/demon_slime/demon_slime.png"});

        int btnWidth = 150;
        int btnHeight = 50;
        int spacing = 20;
        int totalWidth = btnWidth * 2 + spacing;
        int startX = 800 + (400 - totalWidth) / 2;
        int btnY = 510;

        readyBtn = {startX, btnY, btnWidth, btnHeight};
        backBtn = {startX + btnWidth + spacing, btnY, btnWidth, btnHeight};

        if (menuMusic) {
            graphics.play(menuMusic);
        }

        coins = loadCoins();
        sliderBar = {SCREEN_WIDTH / 2 - 150, 250, 300, 10};
        volume = Mix_VolumeMusic(-1) * 100 / 128;
        updateSliderKnob();

        btnRetry = {SCREEN_WIDTH / 2 - 200, 400, 160, 60};
        btnReturnToMenuGameOver = {SCREEN_WIDTH / 2 + 40, 400, 200, 60};

        btnBack = {SCREEN_WIDTH / 2 - 100, 350, 200, 60};
        btnReturnToMenu = {SCREEN_WIDTH / 2 - 100, 420, 200, 60};

        screenShake = false;
        screenShakeStartTime = 0;
        screenShakeDuration = 200;
        screenShakeMagnitude = 5;
        shakeOffsetX = 0;
        shakeOffsetY = 0;

        shopBackground = graphics.loadTexture("assets/imgs/shop.png");
        SDL_Texture* coinTex = graphics.loadTexture("assets/sprites/effects/coin.png");
        coinIcon.initAuto(coinTex, 16, 16, 1);
        startBattleButton = {SCREEN_WIDTH - 220, SCREEN_HEIGHT - 60, 200, 40};
        backShopButton = {20, 20, 100, 40};
        startBattleButtonHovered = false;
        backShopButtonHovered = false;
        shopMessage = "";
        shopMessageTimer = 0;
        initShopItems(graphics);
    }

    void initShopItems(Graphics& graphics) {
        shopItems.clear();
        const int itemWidth = 200;
        const int spacing = 40;
        const int totalItems = 4;
        const int totalWidth = totalItems * itemWidth + (totalItems - 1) * spacing;
        const int startX = (SCREEN_WIDTH - totalWidth) / 2;
        const int itemY = 200;

        ShopItem hpBoost;
        hpBoost.init(graphics, ItemType::HP_BOOST, "Health Potion", "+10 HP", 10, "assets/sprites/items/hp_potion.png", startX, itemY);
        shopItems.push_back(hpBoost);

        ShopItem atkBoost;
        atkBoost.init(graphics, ItemType::ATK_BOOST, "Attack Boost", "+3 ATK", 10, "assets/sprites/items/atk_boost.png", startX + itemWidth + spacing, itemY);
        shopItems.push_back(atkBoost);

        ShopItem speedBoost;
        speedBoost.init(graphics, ItemType::SPEED_BOOST, "Speed Boost", "+0.5 Speed", 15, "assets/sprites/items/speed_boost.png", startX + 2 * (itemWidth + spacing), itemY);
        shopItems.push_back(speedBoost);

        ShopItem shield;
        shield.init(graphics, ItemType::SHIELD, "Shield", "Reduce 20% DMG", 20, "assets/sprites/items/shield.png", startX + 3 * (itemWidth + spacing), itemY);
        shopItems.push_back(shield);
    }

    void updateSliderKnob() {
        int knobX = sliderBar.x + (volume * sliderBar.w / 100) - 5;
        sliderKnob = {knobX, sliderBar.y - 5, 10, 20};
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

            if (state == IN_BATTLE) {
                updateBattle(graphics);
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
        std::ofstream out("assets/database/coins.txt");
        if (out) {
            out << totalCoins << std::endl;
            out.close();
        }
        coins = totalCoins;
    }

    int loadCoins() {
        std::ifstream in("assets/database/coins.txt");
        int coins = 0;
        if (in) {
            in >> coins;
            in.close();
        }
        return coins;
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

    void applyItemEffect(ItemType type) {
        Warrior* warrior = dynamic_cast<Warrior*>(player);
        Archer* archer = dynamic_cast<Archer*>(player);
        if (warrior) {
            switch (type) {
                case ItemType::HP_BOOST:
                    warrior->setHP(warrior->getHP() + 10);
                    break;
                case ItemType::ATK_BOOST:
                    warrior->setAttackDamage(warrior->getAttackDamage() + 3);
                    break;
                case ItemType::SPEED_BOOST:
                    warrior->setSpeed(warrior->getSpeed() + 0.5f);
                    break;
                case ItemType::SHIELD:
                    warrior->setShield(true);
                    break;
                case ItemType::COIN_DROP:
                    break;
            }
        } else if (archer) {
            switch (type) {
                case ItemType::HP_BOOST:
                    archer->setHP(archer->getHP() + 10);
                    break;
                case ItemType::ATK_BOOST:
                    archer->setAttackDamage(archer->getAttackDamage() + 3);
                    break;
                case ItemType::SPEED_BOOST:
                    archer->setSpeed(archer->getSpeed() + 0.5f);
                    break;
                case ItemType::SHIELD:
                    archer->setShield(true);
                    break;
                case ItemType::COIN_DROP:
                    break;
            }
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
                    if (!playerInitialized) {
                        player = new Warrior();
                        player->init(graphics);
                        playerInitialized = true;
                    }
                    loadControls(graphics);
                    showControls = true;
                    controlsDisplayTime = SDL_GetTicks();
                } else if (SDL_PointInRect(&mousePoint, &characterButtons[1].rect)) {
                    graphics.play(clickSound);
                    selectedCharacter = 1;
                    state = PREVIEW_CHARACTER;
                    if (!playerInitialized) {
                        player = new Archer();
                        player->init(graphics);
                        playerInitialized = true;
                    }
                    loadControls(graphics);
                    showControls = true;
                    controlsDisplayTime = SDL_GetTicks();
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
            if (showControls && SDL_GetTicks() - controlsDisplayTime >= controlsDuration) {
                showControls = false;
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
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (readyHovered) {
                    graphics.play(clickSound);
                    state = SHOP;
                } else if (backHovered) {
                    graphics.play(clickSound);
                    state = CHOOSE_STAGE;
                    if (currentBackground) {
                        SDL_DestroyTexture(currentBackground);
                        currentBackground = nullptr;
                    }
                    currentBackground = graphics.loadTexture("assets/imgs/bg.png");
                    if (Mix_PlayingMusic()) {
                        Mix_HaltMusic();
                    }
                    if (menuMusic) {
                        graphics.play(menuMusic);
                    }
                }
            }
        } else if (state == SHOP) {
            startBattleButtonHovered = inRect(mx, my, startBattleButton);
            backShopButtonHovered = inRect(mx, my, backShopButton);

            for (auto& item : shopItems) {
                item.update(mx, my);
            }

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (startBattleButtonHovered) {
                    graphics.play(clickSound);
                    enterBattle(graphics);
                    state = IN_BATTLE;
                } else if (backShopButtonHovered) {
                    graphics.play(clickSound);
                    state = PREVIEW_STAGE;
                } else {
                    for (auto& item : shopItems) {
                        if (item.isClicked(mx, my)) {
                            if (coins >= item.getPrice()) {
                                coins -= item.getPrice();
                                applyItemEffect(item.getType());
                                addCoins(-item.getPrice());
                                graphics.play(clickSound);
                            } else {
                                shopMessage = "Not Enough Coins!";
                                shopMessageTimer = SDL_GetTicks();
                                graphics.play(errorClick);
                            }
                        }
                    }
                }
            }

            if (shopMessageTimer > 0 && SDL_GetTicks() - shopMessageTimer >= 1000) {
                shopMessage = "";
                shopMessageTimer = 0;
            }
        } else if (state == IN_BATTLE) {
            player->handleEvent(e);
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    state = PAUSED;
                } else if (e.key.keysym.sym == SDLK_h) {
                    showControls = !showControls;
                    if (showControls) controlsDisplayTime = SDL_GetTicks();
                }
            }
            if (showControls && SDL_GetTicks() - controlsDisplayTime >= controlsDuration) {
                showControls = false;
            }
        } else if (state == PAUSED) {
            returnToMenuHovered = inRect(mx, my, btnReturnToMenu);
            backPauseHovered = inRect(mx, my, btnBack);
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                state = IN_BATTLE;
                std::ofstream out("assets/database/config.txt");
                if (out) {
                    out << volume << "\n";
                    out << playerName << "\n";
                    out.close();
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (inRect(mx, my, sliderKnob) || inRect(mx, my, sliderBar)) {
                    draggingSlider = true;
                } else if (returnToMenuHovered) {
                    graphics.play(clickSound);
                    fadeOut(graphics);
                    return RETURN_TO_MENU;
                } else if (backPauseHovered) {
                    graphics.play(clickSound);
                    state = IN_BATTLE;
                    std::ofstream out("assets/database/config.txt");
                    if (out) {
                        out << volume << "\n";
                        out << playerName << "\n";
                        out.close();
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                draggingSlider = false;
            } else if (e.type == SDL_MOUSEMOTION && draggingSlider) {
                int relX = mx - sliderBar.x;
                relX = std::max(0, std::min(relX, sliderBar.w));
                volume = relX * 100 / sliderBar.w;
                Mix_VolumeMusic(volume * 128 / 100);
                updateSliderKnob();
            }
        } else if (state == VICTORY) {
            if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
                if (SDL_GetTicks() - resultDisplayTime >= 2000) {
                    fadeOut(graphics);
                    state = PREVIEW_STAGE;
                    if (currentBackground) {
                        SDL_DestroyTexture(currentBackground);
                        currentBackground = nullptr;
                    }
                    currentBackground = graphics.loadTexture("assets/imgs/bg.png");
                    initBackgrounds(graphics);
                    if (Mix_PlayingMusic()) {
                        Mix_HaltMusic();
                    }
                    if (menuMusic) {
                        graphics.play(menuMusic);
                    }
                }
            }
        } else if (state == GAME_OVER) {
            retryHovered = inRect(mx, my, btnRetry);
            returnToMenuGameOverHovered = inRect(mx, my, btnReturnToMenuGameOver);

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (retryHovered) {
                    graphics.play(clickSound);
                    initBackgrounds(graphics);
                    enterBattle(graphics);
                    state = IN_BATTLE;
                    playerDead = false;
                } else if (returnToMenuGameOverHovered) {
                    graphics.play(clickSound);
                    fadeOut(graphics);
                    return RETURN_TO_MENU;
                }
            }
        }
        return state;
    }

    void render(Graphics& graphics) {
        if (state == CHOOSE_CHARACTER || state == CHOOSE_STAGE) {
            SDL_Color titleColor = {255, 255, 0};
            std::string text = (state == CHOOSE_CHARACTER) ? "Choose Your Character" : "Choose Stage";
            SDL_Texture* titleTex = graphics.renderText(text.c_str(), titleFont, titleColor);
            if (titleTex) {
                int tw, th;
                SDL_QueryTexture(titleTex, NULL, NULL, &tw, &th);
                int tx = (SCREEN_WIDTH - tw) / 2;
                int ty = 80;
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
        } else if (state == SHOP) {
            SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(graphics.renderer, shopBackground, NULL, &backgroundRect);

            graphics.renderSprite(SCREEN_WIDTH - 100, 20, coinIcon, false, 1.0f);
            std::string coinText = std::to_string(coins);
            SDL_Texture* coinTex = graphics.renderText(coinText.c_str(), font, normalTextColor);
            if (coinTex) {
                int tw, th;
                SDL_QueryTexture(coinTex, NULL, NULL, &tw, &th);
                graphics.renderTexture(coinTex, SCREEN_WIDTH - 60, 20);
                SDL_DestroyTexture(coinTex);
            }

            for (auto& item : shopItems) {
                item.render(graphics, font, normalTextColor, hoverTextColor);
            }

            SDL_SetRenderDrawColor(graphics.renderer, startBattleButtonHovered ? 100 : 50, startBattleButtonHovered ? 200 : 150, 50, 255);
            SDL_RenderFillRect(graphics.renderer, &startBattleButton);
            SDL_SetRenderDrawColor(graphics.renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(graphics.renderer, &startBattleButton);
            SDL_Texture* startTex = graphics.renderText("Start Battle", font, normalTextColor);
            if (startTex) {
                int tw, th;
                SDL_QueryTexture(startTex, NULL, NULL, &tw, &th);
                graphics.renderTexture(startTex, startBattleButton.x + (startBattleButton.w - tw) / 2, startBattleButton.y + (startBattleButton.h - th) / 2);
                SDL_DestroyTexture(startTex);
            }

            SDL_SetRenderDrawColor(graphics.renderer, backShopButtonHovered ? 100 : 50, backShopButtonHovered ? 200 : 150, 50, 255);
            SDL_RenderFillRect(graphics.renderer, &backShopButton);
            SDL_SetRenderDrawColor(graphics.renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(graphics.renderer, &backShopButton);
            SDL_Texture* backTex = graphics.renderText("Back", font, normalTextColor);
            if (backTex) {
                int tw, th;
                SDL_QueryTexture(backTex, NULL, NULL, &tw, &th);
                graphics.renderTexture(backTex, backShopButton.x + (backShopButton.w - tw) / 2, backShopButton.y + (backShopButton.h - th) / 2);
                SDL_DestroyTexture(backTex);
            }

            if (!shopMessage.empty()) {
                SDL_Texture* msgTex = graphics.renderText(shopMessage.c_str(), font, {255, 0, 0});
                if (msgTex) {
                    int tw, th;
                    SDL_QueryTexture(msgTex, NULL, NULL, &tw, &th);
                    graphics.renderTexture(msgTex, SCREEN_WIDTH / 2 - tw / 2, SCREEN_HEIGHT / 2);
                    SDL_DestroyTexture(msgTex);
                }
            }
        } else if (state == IN_BATTLE || state == PAUSED) {
            renderBattle(graphics);
            if (state == PAUSED) {
                SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 100);
                SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                SDL_RenderFillRect(graphics.renderer, &overlay);

                SDL_Texture* pauseText = graphics.renderText("Paused", font, textColor);
                if (pauseText) {
                    int tw, th;
                    SDL_QueryTexture(pauseText, NULL, NULL, &tw, &th);
                    graphics.renderTexture(pauseText, SCREEN_WIDTH / 2 - tw / 2, 150);
                    SDL_DestroyTexture(pauseText);
                }

                SDL_Texture* volumeLabel = graphics.renderText("Volume", font, textColor);
                if (volumeLabel) {
                    int lw, lh;
                    SDL_QueryTexture(volumeLabel, NULL, NULL, &lw, &lh);
                    graphics.renderTexture(volumeLabel, sliderBar.x - lw - 20, sliderBar.y - lh / 2 + 5);
                    SDL_DestroyTexture(volumeLabel);
                }
                SDL_SetRenderDrawColor(graphics.renderer, 180, 180, 180, 255);
                SDL_RenderFillRect(graphics.renderer, &sliderBar);
                SDL_SetRenderDrawColor(graphics.renderer, 255, 255, 0, 255);
                SDL_RenderFillRect(graphics.renderer, &sliderKnob);

                std::string volStr = std::to_string(volume);
                SDL_Texture* valTex = graphics.renderText(volStr.c_str(), font, {255, 255, 0});
                int vw, vh;
                SDL_QueryTexture(valTex, NULL, NULL, &vw, &vh);
                graphics.renderTexture(valTex, sliderBar.x + sliderBar.w + 20, sliderBar.y - vh / 2 + 5);
                SDL_DestroyTexture(valTex);

                SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
                SDL_Color boxColorBack = backPauseHovered ? SDL_Color{50, 50, 50, 220} : SDL_Color{0, 0, 0, 180};
                SDL_Color textColBack = backPauseHovered ? highlightColor : textColor;
                SDL_SetRenderDrawColor(graphics.renderer, boxColorBack.r, boxColorBack.g, boxColorBack.b, boxColorBack.a);
                SDL_RenderFillRect(graphics.renderer, &btnBack);
                SDL_Texture* backText = graphics.renderText("Back", font, textColBack);
                if (backText) {
                    int tw, th;
                    SDL_QueryTexture(backText, NULL, NULL, &tw, &th);
                    int tx = btnBack.x + (btnBack.w - tw) / 2;
                    int ty = btnBack.y + (btnBack.h - th) / 2;
                    graphics.renderTexture(backText, tx, ty);
                    SDL_DestroyTexture(backText);
                }

                SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
                SDL_Color boxColor = returnToMenuHovered ? SDL_Color{50, 50, 50, 220} : SDL_Color{0, 0, 0, 180};
                SDL_Color textCol = returnToMenuHovered ? highlightColor : textColor;
                SDL_SetRenderDrawColor(graphics.renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
                SDL_RenderFillRect(graphics.renderer, &btnReturnToMenu);
                SDL_Texture* textTex = graphics.renderText("Return to Menu", font, textCol);
                if (textTex) {
                    int tw, th;
                    SDL_QueryTexture(textTex, NULL, NULL, &tw, &th);
                    int tx = btnReturnToMenu.x + (btnReturnToMenu.w - tw) / 2;
                    int ty = btnReturnToMenu.y + (btnReturnToMenu.h - th) / 2;
                    graphics.renderTexture(textTex, tx, ty);
                    SDL_DestroyTexture(textTex);
                }
            }
        } else if (state == VICTORY) {
            renderResult(graphics, "Victory!");
        } else if (state == GAME_OVER) {
            renderResult(graphics, "Game Over!");

            SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
            SDL_Color boxColorRetry = retryHovered ? SDL_Color{50, 50, 50, 220} : SDL_Color{0, 0, 0, 180};
            SDL_Color textColRetry = retryHovered ? hoverTextColor : normalTextColor;
            SDL_SetRenderDrawColor(graphics.renderer, boxColorRetry.r, boxColorRetry.g, boxColorRetry.b, boxColorRetry.a);
            SDL_RenderFillRect(graphics.renderer, &btnRetry);
            SDL_Texture* retryText = graphics.renderText("Retry", font, textColRetry);
            if (retryText) {
                int tw, th;
                SDL_QueryTexture(retryText, NULL, NULL, &tw, &th);
                int tx = btnRetry.x + (btnRetry.w - tw) / 2;
                int ty = btnRetry.y + (btnRetry.h - th) / 2;
                graphics.renderTexture(retryText, tx, ty);
                SDL_DestroyTexture(retryText);
            }

            SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
            SDL_Color boxColorReturn = returnToMenuGameOverHovered ? SDL_Color{50, 50, 50, 220} : SDL_Color{0, 0, 0, 180};
            SDL_Color textColReturn = returnToMenuGameOverHovered ? hoverTextColor : normalTextColor;
            SDL_SetRenderDrawColor(graphics.renderer, boxColorReturn.r, boxColorReturn.g, boxColorReturn.b, boxColorReturn.a);
            SDL_RenderFillRect(graphics.renderer, &btnReturnToMenuGameOver);
            SDL_Texture* returnText = graphics.renderText("Return to Menu", font, textColReturn);
            if (returnText) {
                int tw, th;
                SDL_QueryTexture(returnText, NULL, NULL, &tw, &th);
                int tx = btnReturnToMenuGameOver.x + (btnReturnToMenuGameOver.w - tw) / 2;
                int ty = btnReturnToMenuGameOver.y + (btnReturnToMenuGameOver.h - th) / 2;
                graphics.renderTexture(returnText, tx, ty);
                SDL_DestroyTexture(returnText);
            }
        }
        if (state == PREVIEW_CHARACTER || state == IN_BATTLE) {
            if (showControls) {
                renderControls(graphics);
            }
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
    }

    void enterBattle(Graphics& graphics) {

        if (Mix_PlayingMusic()) {
            Mix_HaltMusic();
        }
        if (battleMusic) {
            graphics.play(battleMusic);
        }
        if (stageBackgrounds.empty()) {
            initBackgrounds(graphics);
        }
        if (selectedStage >= 0 && selectedStage < static_cast<int>(stageBackgrounds.size()) && stageBackgrounds[selectedStage]) {
            if (currentBackground) {
                SDL_DestroyTexture(currentBackground);
                currentBackground = nullptr;
            }
            currentBackground = stageBackgrounds[selectedStage];
        } else {
            if (currentBackground) {
                SDL_DestroyTexture(currentBackground);
                currentBackground = nullptr;
            }
            currentBackground = graphics.loadTexture("assets/imgs/bg.png");
            if (!currentBackground) {
                std::cerr << "Failed to load fallback background: assets/imgs/bg.png" << std::endl;
            }
        }
        if (selectedCharacter == 0) {
            Warrior* warrior = dynamic_cast<Warrior*>(player);
            if (warrior) {
                warrior->setHP(warrior->getHP() > 0 ? warrior->getHP() : 120);
                warrior->setAttackDamage(warrior->getAttackDamage());
                warrior->setState(PlayerState::IDLE);
                warrior->setY(550.0f);
                warrior->getJumpVelocity() = -15.0f;
                warrior->getIsJumping() = false;
            }
        } else {
            Archer* archer = dynamic_cast<Archer*>(player);
            if (archer) {
                archer->getProjectiles().clear();
                archer->setHP(archer->getHP() > 0 ? archer->getHP() : 100);
                archer->setAttackDamage(archer->getAttackDamage());
                archer->setState(PlayerState::IDLE);
                archer->setY(550.0f);
                archer->getJumpVelocity() = -15.0f;
                archer->getIsJumping() = false;
            }
        }


        if (boss) {
            delete boss;
            boss = nullptr;
        }
        if (selectedStage == 0) {
            boss = new FlyingDemon();
            boss->init(graphics, 1000, 550, selectedStage, player->getX());
        } else {
            boss = new DemonSlime();
            boss->init(graphics, 1000, 550, selectedStage, player->getX());
        }
        bossProjectiles.clear();
        asteroids.clear();
        bossDefeated = false;
        chest.isOpened = false;

        screenShake = false;
        screenShakeStartTime = 0;
        shakeOffsetX = 0;
        shakeOffsetY = 0;
    }

    void updateBattle(Graphics& graphics) {
        player->update();
        if (selectedStage == 0) {
            boss->update(player);
            boss->attack(bossProjectiles);
        } else if (selectedStage == 1) {
            boss->update(player);
            boss->attack(bossProjectiles, player);

            static Uint32 lastAsteroidSpawnTime = 0;
            Uint32 currentTime = SDL_GetTicks();

            Uint32 asteroidSpawnInterval = (dynamic_cast<Archer*>(player) != nullptr) ? 1200 : 2000;
            if (currentTime - lastAsteroidSpawnTime >= asteroidSpawnInterval) {
                if (rand() % 100 < 60) {
                    Asteroid asteroid;
                    float randomX = static_cast<float>(rand() % (SCREEN_WIDTH - 96));
                    asteroid.init(graphics, randomX);
                    asteroids.push_back(asteroid);
                }
                lastAsteroidSpawnTime = currentTime;
            }

            for (auto it = asteroids.begin(); it != asteroids.end();) {
                it->update();
                if (!it->isActive()) {
                    if (!screenShake) {
                        screenShake = true;
                        screenShakeStartTime = SDL_GetTicks();
                    }
                    it = asteroids.erase(it);
                } else {
                    SDL_Rect playerRect = {static_cast<int>(player->getX()), static_cast<int>(player->getY()), 96, 84};
                    SDL_Rect asteroidRect = it->getRect();
                    if (SDL_HasIntersection(&playerRect, &asteroidRect)) {
                        player->takeDamage(it->getDamage());
                        if (!screenShake) {
                            screenShake = true;
                            screenShakeStartTime = SDL_GetTicks();
                        }
                        it->setInactive();
                        it = asteroids.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        if (screenShake) {
            Uint32 elapsed = SDL_GetTicks() - screenShakeStartTime;
            if (elapsed < static_cast<Uint32>(screenShakeDuration)) {
                shakeOffsetX = (rand() % (2 * screenShakeMagnitude + 1)) - screenShakeMagnitude;
                shakeOffsetY = (rand() % (2 * screenShakeMagnitude + 1)) - screenShakeMagnitude;
            } else {
                screenShake = false;
                shakeOffsetX = 0;
                shakeOffsetY = 0;
            }
        }

        Warrior* warrior = dynamic_cast<Warrior*>(player);
        if (warrior) {
            float y = warrior->getY();
            float& jumpVelocity = warrior->getJumpVelocity();
            for (const auto& platform : platforms) {
                SDL_Rect playerRect = {static_cast<int>(warrior->getX()), static_cast<int>(y), 96, 84};
                if (SDL_HasIntersection(&playerRect, &platform.rect) && jumpVelocity > 0) {
                    y = platform.rect.y - 155;
                    warrior->setY(y);
                    jumpVelocity = 0;
                    warrior->getIsJumping() = false;
                    warrior->setState(PlayerState::IDLE);
                    break;
                }
            }
            if (warrior->getShouldDealDamage()) {
                float distance = std::abs(warrior->getX() - boss->getX());
                if (distance < 150 && distance >= 50) {
                    boss->takeDamage(warrior->getAttackDamage());
                }
            }
        }

        Archer* archer = dynamic_cast<Archer*>(player);
        if (archer) {
            float y = archer->getY();
            float& jumpVelocity = archer->getJumpVelocity();
            for (const auto& platform : platforms) {
                SDL_Rect playerRect = {static_cast<int>(archer->getX()), static_cast<int>(y), 64, 64};
                if (SDL_HasIntersection(&playerRect, &platform.rect) && jumpVelocity > 0) {
                    y = platform.rect.y - 145;
                    archer->setY(y);
                    jumpVelocity = 0;
                    archer->getIsJumping() = false;
                    archer->setState(PlayerState::IDLE);
                    break;
                }
            }
            for (auto it = archer->getProjectiles().begin(); it != archer->getProjectiles().end();) {
                if (it->active) {
                    SDL_Rect projRect = {
                        static_cast<int>(it->x + (it->facingLeft ? -10 : 10)),
                        static_cast<int>(it->y),
                        static_cast<int>(it->sprite.getWidth() * 2.5f * 0.8f),
                        static_cast<int>(it->sprite.getHeight() * 2.5f)
                    };

                    float bossScale = (dynamic_cast<FlyingDemon*>(boss) != nullptr) ? 2.0f : 2.5f;
                    SDL_Rect bossRect;
                    if (dynamic_cast<FlyingDemon*>(boss)) {
                        bossRect = {
                            static_cast<int>(boss->getX() + 20),
                            static_cast<int>(boss->getY() + 20),
                            boss->getState() == BossState::DEATH ? 0 : static_cast<int>(boss->getSpriteWidth() * bossScale * 0.6f),
                            boss->getState() == BossState::DEATH ? 0 : static_cast<int>(boss->getSpriteHeight() * bossScale * 0.8f)
                        };
                    } else {
                        bossRect = {
                            static_cast<int>(boss->getX() + 100),
                            static_cast<int>(boss->getY() + 150),
                            boss->getState() == BossState::DEATH ? 0 : static_cast<int>(boss->getSpriteWidth() * bossScale * 0.4f),
                            boss->getState() == BossState::DEATH ? 0 : static_cast<int>(boss->getSpriteHeight() * bossScale * 0.5f)
                        };
                    }

                    if (SDL_HasIntersection(&projRect, &bossRect)) {
                        SDL_Log("Arrow hit boss! Boss HP: %d", boss->getHP());
                        boss->takeDamage(archer->getAttackDamage());
                        archer->deactivateProjectile(it);
                        it = archer->getProjectiles().erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    ++it;
                }
            }
        }

        if (selectedStage == 0) {
            for (auto it = bossProjectiles.begin(); it != bossProjectiles.end();) {
                it->x -= 5;
                if (it->x < 0) {
                    it = bossProjectiles.erase(it);
                } else {
                    SDL_Rect playerRect = {static_cast<int>(player->getX()), static_cast<int>(player->getY()), 96, 84};
                    if (SDL_HasIntersection(&(*it), &playerRect)) {
                        player->takeDamage(boss->getAttackDamage());
                        it = bossProjectiles.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        if (selectedStage == 1 && boss->getState() == BossState::ATTACK) {
            float distance = std::abs(player->getX() - boss->getX());
            if (distance >= 50 && distance < 150) {
                static Uint32 lastBossAttackTime = 0;
                Uint32 currentTime = SDL_GetTicks();
                DemonSlime* demonSlime = dynamic_cast<DemonSlime*>(boss);
                if (demonSlime && boss->getCurrentFrame() >= 10 && currentTime - lastBossAttackTime >= 2000) {
                    bool facingLeft = demonSlime->getFacingLeft();
                    float warriorX = player->getX();
                    float demonX = boss->getX();
                    if ((facingLeft && warriorX < demonX) || (!facingLeft && warriorX > demonX)) {
                        player->takeDamage(boss->getAttackDamage());
                        lastBossAttackTime = currentTime;
                    }
                }
            }
        }

        static Uint32 lastAttackTime = 0;
        if (player->getState() == PlayerState::ATTACK1 ||
            player->getState() == PlayerState::ATTACK2 ||
            player->getState() == PlayerState::ATTACK3) {
            float distance = std::abs(player->getX() - boss->getX());
            if (distance >= 50 && distance < 150) {
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - lastAttackTime >= 1000) {
                    boss->takeDamage(player->getAttackDamage());
                    lastAttackTime = currentTime;
                }
            }
        }

        if (boss->getHP() <= 0 && boss->getState() == BossState::DEATH && !bossDefeated) {
            bossDefeated = true;
            bossDefeatedTime = SDL_GetTicks();
        }

        if (bossDefeated && !chest.isOpened) {
            Uint32 currentTime = SDL_GetTicks();
            graphics.play(victorySound);
            if (currentTime - bossDefeatedTime >= 5000) {
                chest.isOpened = true;
                chest.sprite.currentFrame = 2;
                addCoins(10);
                state = VICTORY;
                resultDisplayTime = SDL_GetTicks();
            }
        }

        if (player->getHP() <= 0 && player->getState() == PlayerState::DEATH && !playerDead) {
            playerDead = true;
            playerDeathTime = SDL_GetTicks();
            graphics.play(diedMusic);
        }

        if (playerDead) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - playerDeathTime >= 5000) {
                state = GAME_OVER;
                resultDisplayTime = SDL_GetTicks();
                playerDead = false;
            }
        }
    }

    void renderBattle(Graphics& graphics) {

        SDL_Rect backgroundRect = {shakeOffsetX, shakeOffsetY, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(graphics.renderer, currentBackground, NULL, &backgroundRect);
        for (const auto& platform : platforms) {
            SDL_Rect platformRect = platform.rect;
            platformRect.x += shakeOffsetX;
            platformRect.y += shakeOffsetY;
            graphics.renderTexture(platform.texture, platformRect.x, platformRect.y);
        }
        if (player->getHP() > 0) {
            player->render(graphics, shakeOffsetX, shakeOffsetY);
        }

        boss->render(graphics, shakeOffsetX, shakeOffsetY);
        for (auto& asteroid : asteroids) {
            asteroid.render(graphics, shakeOffsetX, shakeOffsetY);
        }
        if (bossDefeated) {
            graphics.renderSprite(chest.rect.x, chest.rect.y, chest.sprite, false, 1.0f, shakeOffsetX, shakeOffsetY);
            static Uint32 bossDefeatedTimeLocal = 0;
            if (bossDefeated && bossDefeatedTimeLocal == 0) {
                bossDefeatedTimeLocal = SDL_GetTicks();
            }
            Uint32 elapsed = SDL_GetTicks() - bossDefeatedTimeLocal;
            int remainingSeconds = 5 - (elapsed / 1000);
            if (remainingSeconds >= 0) {
                std::string timerText = "Game end in: " + std::to_string(remainingSeconds) + "s";
                SDL_Texture* timerTex = graphics.renderText(timerText.c_str(), font, normalTextColor);
                if (timerTex) {
                    int tw, th;
                    SDL_QueryTexture(timerTex, NULL, NULL, &tw, &th);
                    graphics.renderTexture(timerTex, chest.rect.x + (chest.rect.w - tw) / 2, chest.rect.y - th - 10);
                    SDL_DestroyTexture(timerTex);
                }
            }
        }
        if (playerDead) {
            Uint32 elapsed = SDL_GetTicks() - playerDeathTime;
            int remainingSeconds = 5 - (elapsed / 1000);
            if (remainingSeconds >= 0) {
                std::string timerText = "Game Over in: " + std::to_string(remainingSeconds) + "s";
                SDL_Texture* timerTex = graphics.renderText(timerText.c_str(), font, normalTextColor);
                if (timerTex) {
                    int tw, th;
                    SDL_QueryTexture(timerTex, NULL, NULL, &tw, &th);
                    graphics.renderTexture(timerTex, SCREEN_WIDTH / 2 - tw / 2, SCREEN_HEIGHT / 2 - th - 10);
                    SDL_DestroyTexture(timerTex);
                }
            }
        }
        if (selectedStage == 0) {
            for (const auto& projectile : bossProjectiles) {
                boss->renderProjectile(graphics, projectile);
            }
        }
        renderHealthBars(graphics);
    }

    void renderHealthBars(Graphics& graphics) {
        int baseY = 20;

        SDL_Texture* nameTex = graphics.renderText(playerName.c_str(), font, normalTextColor);
        if (nameTex) {
            int tw, th;
            SDL_QueryTexture(nameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(nameTex, 20, baseY);
            SDL_DestroyTexture(nameTex);
        }

        int healthBarY = baseY + 30;
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
        SDL_SetRenderDrawColor(graphics.renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(graphics.renderer, &playerHealthBar);

        int atkY = healthBarY + 30;
        std::string atkStr = "ATK: " + std::to_string(player->getAttackDamage());
        SDL_Texture* atkTex = graphics.renderText(atkStr.c_str(), font, normalTextColor);
        if (atkTex) {
            int tw, th;
            SDL_QueryTexture(atkTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(atkTex, 20, atkY);
            SDL_DestroyTexture(atkTex);
        }

        Warrior* warrior = dynamic_cast<Warrior*>(player);
        Archer* archer = dynamic_cast<Archer*>(player);
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
            SDL_SetRenderDrawColor(graphics.renderer, 255, 165, 0, 255);
            SDL_RenderFillRect(graphics.renderer, &rageBar);
        } else if (archer) {
            std::string rageStr = "Rage: " + std::to_string(static_cast<int>(archer->getRage())) + "%";
            SDL_Texture* rageTex = graphics.renderText(rageStr.c_str(), font, normalTextColor);
            int rageTextWidth = 0;
            if (rageTex) {
                int tw, th;
                SDL_QueryTexture(rageTex, NULL, NULL, &tw, &th);
                rageTextWidth = tw;
                graphics.renderTexture(rageTex, 20, rageY);
                SDL_DestroyTexture(rageTex);
            }
            SDL_Rect rageBar = {20 + rageTextWidth + 30, rageY, static_cast<int>(archer->getRage() * 2), 10};
            SDL_SetRenderDrawColor(graphics.renderer, 255, 165, 0, 255);
            SDL_RenderFillRect(graphics.renderer, &rageBar);
        }

        int bossBaseY = 20;
        const StagePreview& preview = stagePreviews[selectedStage];
        SDL_Texture* bossNameTex = graphics.renderText(preview.bossName.c_str(), font, normalTextColor);
        if (bossNameTex) {
            int tw, th;
            SDL_QueryTexture(bossNameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(bossNameTex, SCREEN_WIDTH - 20 - tw, bossBaseY);
            SDL_DestroyTexture(bossNameTex);
        }

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
        SDL_SetRenderDrawColor(graphics.renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(graphics.renderer, &bossHealthBar);

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

    void loadControls(Graphics& graphics) {
        controls.clear();
        if (selectedCharacter == 0) {
            controls.push_back({SDL_SCANCODE_A, "Move Left", graphics.loadTexture("assets/sprites/keyboard/A.png")});
            controls.push_back({SDL_SCANCODE_D, "Move Right", graphics.loadTexture("assets/sprites/keyboard/D.png")});
            controls.push_back({SDL_SCANCODE_SPACE, "Jump", graphics.loadTexture("assets/sprites/keyboard/SPACE.png")});
            controls.push_back({SDL_SCANCODE_J, "Attack", graphics.loadTexture("assets/sprites/keyboard/J.png")});
            controls.push_back({SDL_SCANCODE_L, "Defend", graphics.loadTexture("assets/sprites/keyboard/L.png")});
            controls.push_back({SDL_SCANCODE_I, "Rage", graphics.loadTexture("assets/sprites/keyboard/I.png")});
        } else if (selectedCharacter == 1) {
            controls.push_back({SDL_SCANCODE_A, "Move Left", graphics.loadTexture("assets/sprites/keyboard/A.png")});
            controls.push_back({SDL_SCANCODE_D, "Move Right", graphics.loadTexture("assets/sprites/keyboard/D.png")});
            controls.push_back({SDL_SCANCODE_SPACE, "Jump", graphics.loadTexture("assets/sprites/keyboard/SPACE.png")});
            controls.push_back({SDL_SCANCODE_J, "Shoot", graphics.loadTexture("assets/sprites/keyboard/J.png")});
            controls.push_back({SDL_SCANCODE_L, "Roll", graphics.loadTexture("assets/sprites/keyboard/L.png")});
            controls.push_back({SDL_SCANCODE_I, "Rage", graphics.loadTexture("assets/sprites/keyboard/I.png")});
        } else {
            controls.push_back({SDL_SCANCODE_A, "Move Left", graphics.loadTexture("assets/sprites/keyboard/A.png")});
            controls.push_back({SDL_SCANCODE_D, "Move Right", graphics.loadTexture("assets/sprites/keyboard/D.png")});
            controls.push_back({SDL_SCANCODE_SPACE, "Jump", graphics.loadTexture("assets/sprites/keyboard/SPACE.png")});
            controls.push_back({SDL_SCANCODE_J, "Attack", graphics.loadTexture("assets/sprites/keyboard/J.png")});
            controls.push_back({SDL_SCANCODE_L, "Defend/Roll", graphics.loadTexture("assets/sprites/keyboard/L.png")});
            controls.push_back({SDL_SCANCODE_I, "Rage", graphics.loadTexture("assets/sprites/keyboard/I.png")});
        }
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
        if (victorySound) {
            Mix_FreeChunk(victorySound);
            victorySound = nullptr;
        }
        if (diedMusic) {
            Mix_FreeChunk(victorySound);
            diedMusic = nullptr;
        }
        for (auto& platform : platforms) {
            if (platform.texture) {
                SDL_DestroyTexture(platform.texture);
                platform.texture = nullptr;
            }
        }
        for (auto& control : controls) {
            if (control.keyTexture) {
                SDL_DestroyTexture(control.keyTexture);
                control.keyTexture = nullptr;
            }
        }
        controls.clear();
    }
};

#endif
