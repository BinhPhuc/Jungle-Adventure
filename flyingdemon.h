#ifndef _FLYINGDEMON__H
#define _FLYINGDEMON__H

#include "boss.h"

class FlyingDemon : public Boss {
private:
    float speed = 2.0f;
    bool movingRight = false;
    Sprite fireballSprite;
    Uint32 lastAttackTime = 0;
    int attackDamage = 12;

public:
    void init(Graphics& graphics, float startX, float startY, int stage, float warriorX) override {
        x = startX;
        y = startY;
        hp = 90;
        state = BossState::FLYING;

        SDL_Texture* idleTex = graphics.loadTexture("assets/sprites/flying_demon/IDLE.png");
        SDL_Texture* flyingTex = graphics.loadTexture("assets/sprites/flying_demon/FLYING.png");
        SDL_Texture* hurtTex = graphics.loadTexture("assets/sprites/flying_demon/HURT.png");
        SDL_Texture* deathTex = graphics.loadTexture("assets/sprites/flying_demon/DEATH.png");
        SDL_Texture* attackTex = graphics.loadTexture("assets/sprites/flying_demon/ATTACK.png");

        sprites[BossState::IDLE].initAuto(idleTex, 82, 71, 4);
        sprites[BossState::FLYING].initAuto(flyingTex, 82, 71, 4);
        sprites[BossState::HURT].initAuto(hurtTex, 82, 71, 4);
        sprites[BossState::DEATH].initAuto(deathTex, 81, 71, 7);
        sprites[BossState::ATTACK].initAuto(attackTex, 81, 71, 8);

        SDL_Texture* fireballTex = graphics.loadTexture("assets/sprites/flying_demon/fireball.png");
        fireballSprite.initAuto(fireballTex, 64, 32, 5);

        for (auto& pair : sprites) {
            pair.second.frameDelay = 100;
        }
        sprites[BossState::ATTACK].frameDelay = 150;
        sprites[BossState::DEATH].frameDelay = 200;
        fireballSprite.frameDelay = 100;
    }

    void update(Player* player) override {
        if (state == BossState::DEATH) {
            sprites[state].tickTimed(SDL_GetTicks());
            return;
        }

        if (movingRight) {
            x += speed;
            if (x >= 1100) movingRight = false;
        } else {
            x -= speed;
            if (x <= 800) movingRight = true;
        }
        facingLeft = false;

        if (state != BossState::ATTACK && state != BossState::HURT) {
            state = BossState::FLYING;
        }

        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);

        if ((state == BossState::ATTACK && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == BossState::HURT && sprites[state].currentFrame == sprites[state].clips.size() - 1)) {
            state = BossState::FLYING;
        }
    }
    void attack(std::vector<SDL_Rect>& projectiles) override {
        if (state == BossState::DEATH) return;

        Uint32 currentTime = SDL_GetTicks();
        float attackInterval = (hp > 45) ? 2000 : 1000;
        if (currentTime - lastAttackTime >= attackInterval) {
            state = BossState::ATTACK;
            SDL_Rect projectile = {static_cast<int>(x), static_cast<int>(y + 20), 64, 32};
            projectiles.push_back(projectile);
            lastAttackTime = currentTime;
        }
    }

    void attack(std::vector<SDL_Rect>& projectiles, float warriorX) override {
    }

    void attack(std::vector<SDL_Rect>& projectiles, Player* player) override {
    }

    void render(Graphics& graphics) override {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 2.f);
    }
    void render(Graphics& graphics, int offsetX = 0, int offsetY = 0) override {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 2.f, offsetX, offsetY);
    }

    void renderProjectile(Graphics& graphics, const SDL_Rect& projectile, int offsetX = 0, int offsetY = 0) override {
        fireballSprite.tickTimed(SDL_GetTicks());
        graphics.renderSprite(projectile.x, projectile.y, fireballSprite, facingLeft, 1.0f, offsetX, offsetY);
    }

    int getHP() const override { return hp; }
    void takeDamage(int damage) override {
        if (state == BossState::DEATH) return;
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            state = BossState::DEATH;
        } else {
            state = BossState::HURT;
        }
    }
    float getX() const override { return x; }
    float getY() const override { return y; }
    BossState getState() const override { return state; }
    int getAttackDamage() const override { return attackDamage; }
};

#endif
