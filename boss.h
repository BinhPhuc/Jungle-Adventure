#ifndef _BOSS__H
#define _BOSS__H

#include "graphics.h"
#include <vector>
#include <map>

enum class BossState {
    IDLE,
    FLYING,
    HURT,
    DEATH,
    ATTACK
};

class Boss {
protected:
    int hp = 200;
    float x = 1000;
    float y = 500;
    bool facingLeft = false;
    BossState state = BossState::IDLE;
    std::map<BossState, Sprite> sprites;

public:
    virtual void init(Graphics& graphics, float startX, float startY, int stage, float warriorX) = 0;

    virtual void update() {
        // Phương thức ảo để cập nhật trạng thái và vị trí
        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);
    }

//    virtual void update(float warriorX) = 0;

    virtual void update(Player* player) = 0;

    virtual void attack(std::vector<SDL_Rect>& projectiles) {
        // Phương thức ảo để xử lý tấn công
    }

    virtual void attack(std::vector<SDL_Rect>& projectiles, float warriorX) = 0;

    virtual void attack(std::vector<SDL_Rect>& projectiles, Player* player) = 0;

    virtual void render(Graphics& graphics) {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 1.5f);
    }

    virtual void render(Graphics& graphics, int offsetX = 0, int offsetY = 0) = 0;

    virtual void renderProjectile(Graphics& graphics, const SDL_Rect& projectile, int offsetX = 0, int offsetY = 0) = 0;

    virtual int getHP() const { return hp; }
    virtual void takeDamage(int damage) {
        if (state == BossState::DEATH) return;
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            state = BossState::DEATH;
        } else {
            state = BossState::HURT;
        }
    }
    virtual float getX() const { return x; }
    virtual float getY() const { return y; }
    virtual BossState getState() const { return state; }
    virtual int getAttackDamage() const { return 10; }
    int getCurrentFrame() const {
        return sprites.at(state).currentFrame; // Trả về frame hiện tại của trạng thái hiện tại
    }
    int getSpriteWidth() const {
        return sprites.at(state).getWidth();
    }
    int getSpriteHeight() const {
        return sprites.at(state).getHeight();
    }
};

#endif
