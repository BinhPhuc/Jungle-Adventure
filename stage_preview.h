
// --- stage_preview.h ---
#ifndef STAGE_PREVIEW_H
#define STAGE_PREVIEW_H

#include "graphics.h"
#include <SDL.h>
#include <string>
#include <vector>

struct StageInfo {
    std::string name;
    std::string description;
    SDL_Texture* previewImage;
    SDL_Rect buttonRect;
};

class StagePreviewUI {
private:
    TTF_Font* titleFont;
    TTF_Font* descFont;
    SDL_Texture* readyBtnTex;
    SDL_Texture* backBtnTex;
    SDL_Rect readyBtnRect;
    SDL_Rect backBtnRect;

    int hoveredIndex = -1;
    bool hoveringReady = false;
    bool hoveringBack = false;

public:
    std::vector<StageInfo> stages;
    int selectedStage = -1;

    void setStage(int index) {
        selectedStage = index;
    }

    void init(Graphics& graphics) {
        titleFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 36);
        descFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 20);
        std::cout << titleFont << " " << descFont << "\n";

        readyBtnTex = graphics.renderText("READY", titleFont, {255, 255, 255});
        backBtnTex = graphics.renderText("BACK", titleFont, {255, 255, 255});

        readyBtnRect = {900, 500, 150, 50};
        backBtnRect = {900, 570, 150, 50};

        // Stage buttons on the left
        for (int i = 0; i < (int)stages.size(); i++) {
            stages[i].buttonRect = {100, 200 + i * 80, 250, 60};
        }
    }

    void render(Graphics& graphics) {
        // Draw stage buttons
        for (int i = 0; i < (int)stages.size(); i++) {
            SDL_Color color = (i == hoveredIndex) ? SDL_Color{100, 255, 100} : SDL_Color{255, 255, 255};
//            std::cout << stages[i].name << "\n";
            SDL_Texture* btnText = graphics.renderText(stages[i].name.c_str(), titleFont, color);
            graphics.renderRect(stages[i].buttonRect, color, false);
            graphics.renderTexture(btnText, stages[i].buttonRect.x + 20, stages[i].buttonRect.y + 10);
            SDL_DestroyTexture(btnText);
        }

        if (selectedStage != -1) {
            // Draw preview box
            SDL_Rect previewBox = {600, 120, 600, 450};
            graphics.renderRect(previewBox, {50, 50, 50}, true);

            // Draw image
            SDL_Texture* img = stages[selectedStage].previewImage;
            graphics.renderTexture(img, previewBox.x + 250, previewBox.y + 60);

            // Draw name + desc
            SDL_Texture* nameTex = graphics.renderText(stages[selectedStage].name.c_str(), titleFont, {255, 100, 0});
            graphics.renderTexture(nameTex, previewBox.x + 20, previewBox.y + 20);
            SDL_DestroyTexture(nameTex);

            SDL_Texture* descTex = graphics.renderText(stages[selectedStage].description.c_str(), descFont, {255, 255, 255});
            graphics.renderTexture(descTex, previewBox.x + 20, previewBox.y + 80);
            SDL_DestroyTexture(descTex);

            // Buttons
            SDL_Color readyColor = hoveringReady ? SDL_Color{100, 255, 100} : SDL_Color{255, 255, 255};
            SDL_Color backColor = hoveringBack ? SDL_Color{100, 255, 100} : SDL_Color{255, 255, 255};

            SDL_Texture* readyText = graphics.renderText("READY", titleFont, readyColor);
            SDL_Texture* backText = graphics.renderText("BACK", titleFont, backColor);

            graphics.renderRect(readyBtnRect, readyColor, false);
            graphics.renderRect(backBtnRect, backColor, false);
            graphics.renderTexture(readyText, readyBtnRect.x + 30, readyBtnRect.y + 10);
            graphics.renderTexture(backText, backBtnRect.x + 30, backBtnRect.y + 10);

            SDL_DestroyTexture(readyText);
            SDL_DestroyTexture(backText);
        }
    }

    std::string handleEvent(const SDL_Event& e) {
        int mx, my;
        if (e.type == SDL_MOUSEMOTION) {
            mx = e.motion.x;
            my = e.motion.y;
            hoveredIndex = -1;
            hoveringReady = hoveringBack = false;

            for (int i = 0; i < (int)stages.size(); i++) {
                SDL_Point point = {mx, my};
                if (SDL_PointInRect(&point, &stages[i].buttonRect)) {
                    hoveredIndex = i;
                }
            }
            SDL_Point point = {mx, my};
            if (SDL_PointInRect(&point, &readyBtnRect)) hoveringReady = true;
            if (SDL_PointInRect(&point, &backBtnRect)) hoveringBack = true;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            mx = e.button.x;
            my = e.button.y;

            for (int i = 0; i < (int)stages.size(); i++) {
                SDL_Point point = {mx, my};
                if (SDL_PointInRect(&point, &stages[i].buttonRect)) {
                    selectedStage = i;
                    return "preview";
                }
            }
            SDL_Point point = {mx, my};
            if (SDL_PointInRect(&point, &readyBtnRect)) return "ready";
            if (SDL_PointInRect(&point, &backBtnRect)) return "back";
        }

        return "none";
    }
};

#endif
