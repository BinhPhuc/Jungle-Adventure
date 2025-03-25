#ifndef _MENU__H
#define _MENU__H

#include <SDL.h>
#include <string>
#include <vector>

struct MenuButton {
    std::string text;
    SDL_Rect rect;
    bool isHovered = false;
};

enum MenuResult {
    MENU_NONE,
    MENU_START,
    MENU_OPTION,
    MENU_QUIT
};

class Menu {
private:
    std::vector<MenuButton> buttons;
    TTF_Font* font = nullptr;
    TTF_Font* titleFont = nullptr;
    std::string gameTitle = "Jungle Adventure";
    SDL_Cursor* arrowCursor = nullptr;
    SDL_Cursor* handCursor = nullptr;
    SDL_Color titleColor = {255, 255, 0};
    SDL_Color normalTextColor = {255, 255, 255};
    SDL_Color hoverTextColor = {100, 255, 100};
    SDL_Color normalBoxColor = {0, 0, 0, 180};
    SDL_Color hoverBoxColor = {50, 50, 50, 220};
public:
    void init(Graphics& graphics) {
        titleFont = graphics.loadFont("assets/font/Coiny-Regular.ttf", 64);
        font = graphics.loadFont("assets/font/Coiny-Regular.ttf", 48);
        int buttonW = 240;
        int buttonH = 70;
        int x = (SCREEN_WIDTH - buttonW) / 2;

        buttons.push_back({"START", {x, 250, buttonW, buttonH}});
        buttons.push_back({"OPTION", {x, 350, buttonW, buttonH}});

        arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        SDL_SetCursor(arrowCursor);
    }

    void render(Graphics& graphics) {
        SDL_Texture* titleTex = graphics.renderText(gameTitle.c_str(), titleFont, titleColor);
        if (titleTex) {
            int tw, th;
            SDL_QueryTexture(titleTex, NULL, NULL, &tw, &th);
            int tx = (SCREEN_WIDTH - tw) / 2;
            int ty = 80;
            graphics.renderTexture(titleTex, tx, ty);
            SDL_DestroyTexture(titleTex);
        }


        for (auto& btn : buttons) {
            SDL_Color boxColor = btn.isHovered ? hoverBoxColor : normalBoxColor;
            SDL_Color textColor = btn.isHovered ? hoverTextColor : normalTextColor;
            SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(graphics.renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
            SDL_RenderFillRect(graphics.renderer, &btn.rect);
            SDL_Texture* textTex = graphics.renderText(btn.text.c_str(), font, textColor);
            if (textTex) {
                int tw, th;
                SDL_QueryTexture(textTex, NULL, NULL, &tw, &th);
                int tx = btn.rect.x + (btn.rect.w - tw) / 2;
                int ty = btn.rect.y + (btn.rect.h - th) / 2;
                graphics.renderTexture(textTex, tx, ty);
                SDL_DestroyTexture(textTex);
            }
        }
    }

    MenuResult handleEvent(SDL_Event& e, Graphics& graphics, Mix_Chunk* clickSound) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        bool anyHovered = false;
        for (auto& btn : buttons) {
            btn.isHovered = inRect(mx, my, btn.rect);
            if (btn.isHovered) anyHovered = true;
        }
        SDL_SetCursor(anyHovered ? handCursor : arrowCursor);

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (buttons[0].isHovered) {
                graphics.play(clickSound);
                return MENU_START;
            }
            if (buttons[1].isHovered) {
                graphics.play(clickSound);
                return MENU_OPTION;
            }
        }
        return MENU_NONE;
    }

    void renderOverlay(Graphics& graphics) {
        SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 100);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(graphics.renderer, &overlay);
    }

    bool inRect(int x, int y, SDL_Rect rect) {
        return x >= rect.x && x <= rect.x + rect.w &&
               y >= rect.y && y <= rect.y + rect.h;
    }

    void quit() {
        SDL_FreeCursor(arrowCursor);
        SDL_FreeCursor(handCursor);
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        if (titleFont) {
            TTF_CloseFont(titleFont);
            titleFont = nullptr;
        }
    }
};
#endif
