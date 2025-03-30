#ifndef _ASTEROID__H
#define _ASTEROID__H

#include "graphics.h"

class Asteroid {
private:
    float x, y;
    float vy;
    Sprite sprite;
    bool active;
    int damage;

public:
    Asteroid() : x(0), y(0), vy(10.0f), active(true), damage(10) {}

    void init(Graphics& graphics, float startX) {
        x = startX;
        y = -96.0f;
        vy = 10.0f;
        active = true;
        damage = 10;

        SDL_Texture* asteroidTex = graphics.loadTexture("assets/sprites/effects/asteroid.png");
        sprite.initAuto(asteroidTex, 96, 96, 8);
        sprite.frameDelay = 100;
    }

    void update() {
        if (!active) return;

        y += vy;
        sprite.tickTimed(SDL_GetTicks());

        if (y > SCREEN_HEIGHT) {
            active = false;
        }
    }

    void render(Graphics& graphics, int offsetX = 0, int offsetY = 0) {
        if (!active) return;
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprite, false, 1.0f, offsetX, offsetY);
    }

    bool isActive() const { return active; }

    void setInactive() { active = false; }

    SDL_Rect getRect() const {
        return {static_cast<int>(x), static_cast<int>(y), 96, 96};
    }

    int getDamage() const { return damage; }
};

#endif
