#ifndef _OPTION__H
#define _OPTION__H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <fstream>
#include "graphics.h"

enum OptionResult {
    OPTION_BACK,
    OPTION_NONE
};

class OptionMenu {
private:
    TTF_Font* font = nullptr;
    SDL_Color textColor = {255, 255, 255};
    SDL_Color highlightColor = {100, 255, 100};

    SDL_Rect btnBack, btnSave;
    SDL_Rect sliderBar, sliderKnob;
    SDL_Rect nameInputBox;

    bool dragging = false;
    bool backHovered = false;
    bool saveHovered = false;
    bool nameBoxHovered = false;
    bool typingName = false;

    int volume = 50;
    std::string playerName = "PLayer 1";

    bool saveConfirmed = false;
    Uint32 saveConfirmTime = 0;

public:
    void init(Graphics& graphics) {
        font = graphics.loadFont("assets/font/Coiny-Regular.ttf", 36);
        sliderBar = { SCREEN_WIDTH / 2 - 150, 250, 300, 10 };
        int totalWidth = 160 * 2 + 20;
        int startX = (SCREEN_WIDTH - totalWidth) / 2;

        btnBack = { startX, 400, 160, 60 };
        btnSave = { startX + 160 + 20, 400, 160, 60 };
        nameInputBox = {SCREEN_WIDTH / 2 - 150, 320, 300, 40};

        loadSettings();
        updateSliderKnob();
        SDL_StartTextInput();
    }

    void updateSliderKnob() {
        int knobX = sliderBar.x + (volume * sliderBar.w / 100) - 5;
        sliderKnob = { knobX, sliderBar.y - 5, 10, 20 };
    }

    OptionResult handleEvent(SDL_Event& e, Graphics& graphics, Mix_Chunk* clickSound) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        backHovered = inRect(mx, my, btnBack);
        saveHovered = inRect(mx, my, btnSave);
        nameBoxHovered = inRect(mx, my, nameInputBox);

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (inRect(mx, my, sliderKnob) || inRect(mx, my, sliderBar)) {
                dragging = true;
            } else if (backHovered) {
                graphics.play(clickSound);

                return OPTION_BACK;
            } else if (saveHovered) {
                graphics.play(clickSound);
                saveSettings();
                saveConfirmed = true;
                saveConfirmTime = SDL_GetTicks();
            } else if (nameBoxHovered) {
                typingName = true;
            } else {
                typingName = false;
            }
        }
        else if (e.type == SDL_MOUSEBUTTONUP) {
            dragging = false;
        }
        else if (e.type == SDL_MOUSEMOTION && dragging) {
            int relX = mx - sliderBar.x;
            relX = std::max(0, std::min(relX, sliderBar.w));
            volume = relX * 100 / sliderBar.w;
            Mix_VolumeMusic(volume * 128 / 100);
            updateSliderKnob();
        }
        else if (typingName && e.type == SDL_TEXTINPUT) {
            if (playerName.length() < 16) {
                playerName += e.text.text;
            }
        }
        else if (typingName && e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !playerName.empty()) {
                playerName.pop_back();
            }
        }
        if (saveConfirmed && SDL_GetTicks() - saveConfirmTime > 2000) {
            saveConfirmed = false;
        }
        return OPTION_NONE;
    }


    void renderOverlay(Graphics& graphics) {
        SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 100);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(graphics.renderer, &overlay);
    }

    void renderButton(Graphics& graphics, SDL_Rect rect, const std::string& label, bool hovered) {
        SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
        SDL_Color boxColor = hovered ? SDL_Color{50, 50, 50, 220} : SDL_Color{0, 0, 0, 180};
        SDL_Color textCol = hovered ? highlightColor : textColor;
        SDL_SetRenderDrawColor(graphics.renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_RenderFillRect(graphics.renderer, &rect);
        SDL_Texture* textTex = graphics.renderText(label.c_str(), font, textCol);
        if (textTex) {
            int tw, th;
            SDL_QueryTexture(textTex, NULL, NULL, &tw, &th);
            int tx = rect.x + (rect.w - tw) / 2;
            int ty = rect.y + (rect.h - th) / 2;
            graphics.renderTexture(textTex, tx, ty);
            SDL_DestroyTexture(textTex);
        }
    }

    void render(Graphics& graphics) {
        renderOverlay(graphics);
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
        SDL_Texture* nameLabel = graphics.renderText("Player", font, textColor);
        if (nameLabel) {
            int lw, lh;
            SDL_QueryTexture(nameLabel, NULL, NULL, &lw, &lh);
            graphics.renderTexture(nameLabel, nameInputBox.x - lw - 20, nameInputBox.y + (nameInputBox.h - lh) / 2);
            SDL_DestroyTexture(nameLabel);
        }
        SDL_SetRenderDrawColor(graphics.renderer, 50, 50, 50, 200);
        SDL_RenderFillRect(graphics.renderer, &nameInputBox);
        SDL_Texture* nameText = graphics.renderText(playerName.c_str(), font, textColor);
        if (nameText) {
            graphics.renderTexture(nameText, nameInputBox.x + 10, nameInputBox.y + 5);
            SDL_DestroyTexture(nameText);
        }
        renderButton(graphics, btnBack, "Back", backHovered);
        if (saveConfirmed) {
            renderButton(graphics, btnSave, "Saved!", saveHovered);
        } else {
            renderButton(graphics, btnSave, "Save", saveHovered);
        }
    }

    void saveSettings() {
        if (playerName.empty()) {
            playerName = "Player 1";
        }
        std::ofstream out("config.txt");
        if (out) {
            out << volume << "\n";
            out << playerName << "\n";
            out.close();
        }
    }

    void loadSettings() {
        std::ifstream in("config.txt");
        if (in) {
            in >> volume;
            in.ignore();
            std::getline(in, playerName);
            in.close();
            Mix_VolumeMusic(volume * 128 / 100);
        }
    }

    bool inRect(int x, int y, SDL_Rect rect) {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    void quit() {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        SDL_StopTextInput();
    }
};

#endif
