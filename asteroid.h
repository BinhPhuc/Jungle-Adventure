#ifndef _ASTEROID__H
#define _ASTEROID__H

#include "graphics.h"

class Asteroid {
private:
    float x, y; // Vị trí của thiên thạch
    float vy; // Vận tốc rơi (theo trục y)
    Sprite sprite; // Sprite của thiên thạch
    bool active; // Trạng thái hoạt động (true nếu còn rơi, false nếu đã chạm đất hoặc trúng Warrior)
    int damage; // Sát thương gây ra khi trúng Warrior

public:
    Asteroid() : x(0), y(0), vy(10.0f), active(true), damage(10) {}

    void init(Graphics& graphics, float startX) {
        x = startX; // Vị trí x ngẫu nhiên
        y = -96.0f; // Bắt đầu từ trên màn hình (ngoài màn hình)
        vy = 10.0f; // Tốc độ rơi
        active = true;
        damage = 10; // Sát thương mặc định

        // Tải sprite sheet của asteroid
        SDL_Texture* asteroidTex = graphics.loadTexture("assets/sprites/effects/asteroid.png");
        sprite.initAuto(asteroidTex, 96, 96, 8); // frameWidth=96, frameHeight=96, frameCount=8
        sprite.frameDelay = 100; // Tốc độ animation
    }

    void update() {
        if (!active) return;

        // Cập nhật vị trí (rơi xuống)
        y += vy;

        // Cập nhật animation
        sprite.tickTimed(SDL_GetTicks());

        // Nếu thiên thạch rơi xuống dưới màn hình, đánh dấu không hoạt động
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
