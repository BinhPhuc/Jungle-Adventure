#ifndef BOSS_H
#define BOSS_H

#include "graphics.h"
#include <SDL.h>
#include <map>

enum BossState {
    BOSS_IDLE,
    BOSS_WALK
};

class Boss {
private:
    int x = 1000;
    int y = 500;
    int vx = -2;
    bool facingLeft = true;

    BossState state = BOSS_WALK;
    std::map<BossState, Sprite> sprites;

public:
    void init(Graphics& graphics, int startX, int startY, int stage) {
        x = startX;
        y = startY;

        if (stage == 0) {
            SDL_Texture* idleTex = graphics.loadTexture("assets/boss/IDLE.png");
            SDL_Texture* walkTex = graphics.loadTexture("assets/boss/FLYING.png");
            sprites[BOSS_IDLE].initAuto(idleTex, 81, 71, 4);
            sprites[BOSS_WALK].initAuto(walkTex, 81, 71, 4);
        } else {
            SDL_Texture* idleTex = graphics.loadTexture("assets/boss/boss2_idle.png");
            SDL_Texture* walkTex = graphics.loadTexture("assets/boss/boss2_walk.png");
            sprites[BOSS_IDLE].initAuto(idleTex, 90, 90, 4);
            sprites[BOSS_WALK].initAuto(walkTex, 90, 90, 6);
        }
    }

    void update() {
        x += vx;

        // Đổi hướng nếu chạm rìa trái/phải màn hình
        if (x <= 800) {
            vx = 2;
            facingLeft = false;
        } else if (x >= 1100) {
            vx = -2;
            facingLeft = true;
        }

        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);
    }

    void render(Graphics& graphics) {
        graphics.renderSprite(x, y, sprites[state], facingLeft, 1.5f);
    }
};

#endif

