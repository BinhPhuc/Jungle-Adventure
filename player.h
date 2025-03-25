
#ifndef PLAYER_H
#define PLAYER_H

#include "graphics.h"
#include <SDL.h>
#include <map>

enum PlayerState {
    PLAYER_IDLE,
    PLAYER_WALK,
    // sau sẽ thêm ATTACK, JUMP, DEAD, v.v.
};

class Player {
private:
    int x = 100;
    int y = 500;
    int vx = 0;
    bool facingLeft = false;

    PlayerState state = PLAYER_IDLE;
    std::map<PlayerState, Sprite> sprites;

public:
    void init(Graphics& graphics) {
        SDL_Texture* idleTex = graphics.loadTexture("assets/player/Idle.png");
        SDL_Texture* walkTex = graphics.loadTexture("assets/player/Walk.png");
        sprites[PLAYER_IDLE].initAuto(idleTex, 72, 86, 4);
        sprites[PLAYER_WALK].initAuto(walkTex, 72, 86, 8);
    }

    void handleEvent(const SDL_Event& e) {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            if (e.key.keysym.sym == SDLK_LEFT) {
                vx = -4;
                facingLeft = true;
            } else if (e.key.keysym.sym == SDLK_RIGHT) {
                vx = 4;
                facingLeft = false;
            }
        } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT) {
                vx = 0;
            }
        }
    }

    void update() {
        x += vx;
        state = (vx == 0) ? PLAYER_IDLE : PLAYER_WALK;

        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);
    }

    void render(Graphics& graphics) {
        graphics.renderSprite(x, y, sprites[state], facingLeft, 1.5f);
    }
};

#endif
