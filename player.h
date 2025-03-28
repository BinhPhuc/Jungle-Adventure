#ifndef _PLAYER__H
#define _PLAYER__H

#include "graphics.h"
#include <SDL.h>
#include <map>
#include <vector>

enum class PlayerState {
    IDLE,
    WALK,
    RUN,
    JUMP,
    HURT,
    ATTACK1,
    ATTACK2,
    ATTACK3,
    DEFEND,
    DEATH
};

struct Projectile {
    float x, y;
    bool facingLeft;
    Sprite sprite;
    bool active = true;

    Projectile(float _x, float _y, bool _facingLeft) : x(_x), y(_y), facingLeft(_facingLeft) {}
};

class Player {
protected:
    float x = 100;
    float y = 600;
    bool facingLeft = false;
    int hp = 150;
    PlayerState state = PlayerState::IDLE;
    std::map<PlayerState, Sprite> sprites;
    bool isDefending = false;

public:
    virtual void init(Graphics& graphics) {
    }

    virtual void handleEvent(const SDL_Event& e) {
    }

    virtual void render(Graphics& graphics, int offsetX = 0, int offsetY = 0) {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 2.5f, offsetX, offsetY);
    }

    virtual void update() {
        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);
    }

    virtual void render(Graphics& graphics) {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 2.5f);
    }

    virtual void attack(std::vector<SDL_Rect>& projectiles) {
    }

    virtual int getHP() const { return hp; }
    virtual void setHP(int newHP) { hp = newHP; }
    virtual void takeDamage(int damage) {
        if (state == PlayerState::DEATH) return;
        if (!isDefending) {
            hp -= damage;
            if (hp <= 0) {
                hp = 0;
                state = PlayerState::DEATH;
            } else {
                state = PlayerState::HURT;
            }
        }
    }
    virtual float getX() const { return x; }
    virtual float getY() const { return y; }
    virtual void setY(float newY) { y = newY; }
    virtual PlayerState getState() const { return state; }
    virtual int getAttackDamage() const { return 10; }
    bool getIsDefending() const { return isDefending; }
    void setIsDefending(bool defending) { isDefending = defending; }
    virtual std::vector<Projectile>& getProjectiles() = 0;
    virtual const std::vector<Projectile>& getProjectiles() const = 0;
    virtual void deactivateProjectile(std::vector<Projectile>::iterator it) = 0;
};

#endif
