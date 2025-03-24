#ifndef _KNIGHT__H
#define _KNIGHT__H

#include "graphics.h"
#include "camera.h"
#include <SDL.h>
#include <map>
#include <string>

enum KnightState {
    KNIGHT_IDLE,
    KNIGHT_WALK
};

class Knight {
private:
    int x, y;
    int vx = 0;
    int vy = 0;
    int groundY = SCREEN_HEIGHT - 120 * 1.5;
    int speed = 5;
    bool flip = false;
    KnightState state = KNIGHT_IDLE;

    std::map<KnightState, Sprite> animations;

public:
    void init(Graphics& graphics) {
        SDL_Texture* idleTex = graphics.loadTexture("assets/knight_sprites/Idle.png");
        SDL_Texture* walkTex = graphics.loadTexture("assets/knight_sprites/Walk.png");

        animations[KNIGHT_IDLE].initAuto(idleTex, 72, 86, 4);
        animations[KNIGHT_WALK].initAuto(walkTex, 72, 86, 8);

        x = 100;
        y = -200;
    }

    void handleEvent(const SDL_Event& e) {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    vx -= speed;
                    flip = true;
                    break;
                case SDLK_RIGHT:
                    vx += speed;
                    flip = false;
                    break;
            }
        } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    vx += speed;
                    break;
                case SDLK_RIGHT:
                    vx -= speed;
                    break;
            }
        }
    }

    void update(const Camera& cam) {
        if (y < groundY) {
            vy += 1;
            y += vy;
            if (y >= groundY) {
                y = groundY;
                vy = 0;
            }
        } else {
            if (vx < 0 && x + vx >= 0) {
                x += vx;
            } else if (vx > 0) {
                if (x <= cam.x + SCREEN_WIDTH * 0.7f) {
                    x += vx;
                }
            }
        }

        state = (vx != 0) ? KNIGHT_WALK : KNIGHT_IDLE;

        Uint32 now = SDL_GetTicks();
        animations[state].tickTimed(now);
    }

    void render(Graphics& graphics, const Camera& cam) {
        graphics.renderSprite(x, y, animations[state], cam, flip, 1.5f);
    }

    int getX() const { return x; }
    int getWidth() const {
        return animations.at(state).getWidth() * 1.5;
    }
};

#endif
