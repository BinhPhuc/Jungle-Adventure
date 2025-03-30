#ifndef _SHOPITEM__H
#define _SHOPITEM__H

#include "graphics.h"

enum class ItemType {
    HP_BOOST,
    ATK_BOOST,
    SPEED_BOOST,
    SHIELD,
    COIN_DROP
};

class ShopItem {
private:
    ItemType type;
    std::string name;
    std::string description;
    int price;
    Sprite icon;
    SDL_Rect buyButtonRect;
    bool isHovered;

public:
    ShopItem() : type(ItemType::HP_BOOST), name(""), description(""), price(0), isHovered(false) {
        buyButtonRect = {0, 0, 140, 50};
    }

    void init(Graphics& graphics, ItemType t, const std::string& n, const std::string& desc, int p, const std::string& iconPath, int x, int y) {
        type = t;
        name = n;
        description = desc;
        price = p;
        SDL_Texture* iconTex = graphics.loadTexture(iconPath.c_str());
        icon.initAuto(iconTex, 32, 32, 1);
        buyButtonRect = {x + 40, y + 160, 140, 50};
        isHovered = false;
    }

    ItemType getType() const { return type; }
    int getPrice() const { return price; }

    void update(int mouseX, int mouseY) {
        isHovered = (mouseX >= buyButtonRect.x && mouseX <= buyButtonRect.x + buyButtonRect.w &&
                     mouseY >= buyButtonRect.y && mouseY <= buyButtonRect.y + buyButtonRect.h);
    }

    bool isClicked(int mouseX, int mouseY) const {
        return isHovered && (mouseX >= buyButtonRect.x && mouseX <= buyButtonRect.x + buyButtonRect.w &&
                             mouseY >= buyButtonRect.y && mouseY <= buyButtonRect.y + buyButtonRect.h);
    }

    void render(Graphics& graphics, TTF_Font* font, SDL_Color normalTextColor, SDL_Color hoverTextColor) {
        SDL_Rect cardRect = {buyButtonRect.x - 40, buyButtonRect.y - 160, 220, 200};
        SDL_SetRenderDrawColor(graphics.renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(graphics.renderer, &cardRect);
        SDL_SetRenderDrawColor(graphics.renderer, 255, 215, 0, 255);
        SDL_RenderDrawRect(graphics.renderer, &cardRect);

        graphics.renderSprite(cardRect.x + (cardRect.w - 32) / 2, cardRect.y + 15, icon, false, 1.0f);

        SDL_Texture* nameTex = graphics.renderText(name.c_str(), font, normalTextColor);
        if (nameTex) {
            int tw, th;
            SDL_QueryTexture(nameTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(nameTex, cardRect.x + (cardRect.w - tw) / 2, cardRect.y + 60);
            SDL_DestroyTexture(nameTex);
        }

        SDL_Texture* descTex = graphics.renderText(description.c_str(), font, {200, 200, 200});
        if (descTex) {
            int tw, th;
            SDL_QueryTexture(descTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(descTex, cardRect.x + (cardRect.w - tw) / 2, cardRect.y + 90);
            SDL_DestroyTexture(descTex);
        }

        std::string priceText = std::to_string(price) + " Coins";
        SDL_Texture* priceTex = graphics.renderText(priceText.c_str(), font, {255, 215, 0});
        if (priceTex) {
            int tw, th;
            SDL_QueryTexture(priceTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(priceTex, cardRect.x + (cardRect.w - tw) / 2, cardRect.y + 120);
            SDL_DestroyTexture(priceTex);
        }

        SDL_SetRenderDrawColor(graphics.renderer, isHovered ? 100 : 50, isHovered ? 200 : 150, 50, 255);
        SDL_RenderFillRect(graphics.renderer, &buyButtonRect);
        SDL_SetRenderDrawColor(graphics.renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(graphics.renderer, &buyButtonRect);
        SDL_Texture* buyTex = graphics.renderText("Buy", font, normalTextColor);
        if (buyTex) {
            int tw, th;
            SDL_QueryTexture(buyTex, NULL, NULL, &tw, &th);
            graphics.renderTexture(buyTex, buyButtonRect.x + (buyButtonRect.w - tw) / 2, buyButtonRect.y + (buyButtonRect.h - th) / 2);
            SDL_DestroyTexture(buyTex);
        }
    }
};

#endif
