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
private:
    int hp = 200;
    float x = 1000;
    float y = 500;
    float speed = 2.0f;
    bool movingRight = false;
    bool facingLeft = true;
    BossState state = BossState::FLYING;
    std::map<BossState, Sprite> sprites;
    Sprite fireballSprite;
    Uint32 lastAttackTime = 0;
    int attackDamage = 10;

public:
    void init(Graphics& graphics, float startX, float startY, int stage) {
        x = startX;
        y = startY;
        hp = 200;

        // Tải sprite cho từng hành động từ các file riêng biệt
        SDL_Texture* idleTex = graphics.loadTexture("assets/sprites/flying_demon/IDLE.png");
        SDL_Texture* flyingTex = graphics.loadTexture("assets/sprites/flying_demon/FLYING.png");
        SDL_Texture* hurtTex = graphics.loadTexture("assets/sprites/flying_demon/HURT.png");
        SDL_Texture* deathTex = graphics.loadTexture("assets/sprites/flying_demon/DEATH.png");
        SDL_Texture* attackTex = graphics.loadTexture("assets/sprites/flying_demon/ATTACK.png");

        // Khởi tạo sprite cho từng hành động
        sprites[BossState::IDLE].initAuto(idleTex, 82, 71, 4);
        sprites[BossState::FLYING].initAuto(flyingTex, 82, 71, 4);
        sprites[BossState::HURT].initAuto(hurtTex, 82, 71, 4);
        sprites[BossState::DEATH].initAuto(deathTex, 81, 71, 7);
        sprites[BossState::ATTACK].initAuto(attackTex, 81, 71, 8);

        // Tải sprite quả cầu lửa
        SDL_Texture* fireballTex = graphics.loadTexture("assets/sprites/flying_demon/fireball.png");
        fireballSprite.initAuto(fireballTex, 32, 32, 1);

        // Điều chỉnh tốc độ animation
        for (auto& pair : sprites) {
            pair.second.frameDelay = 100;
        }
        sprites[BossState::ATTACK].frameDelay = 150;
        sprites[BossState::DEATH].frameDelay = 200;
        fireballSprite.frameDelay = 100;
    }

    void update() {
        if (state == BossState::DEATH) {
            sprites[state].tickTimed(SDL_GetTicks());
            return;
        }

        // Di chuyển qua lại
        if (movingRight) {
            x += speed;
            if (x >= 1100) movingRight = false;
        } else {
            x -= speed;
            if (x <= 800) movingRight = true;
        }
        facingLeft = !movingRight;

        // Cập nhật trạng thái
        if (state != BossState::ATTACK && state != BossState::HURT) {
            state = BossState::FLYING;
        }

        // Cập nhật animation
        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);

        // Quay lại trạng thái FLYING sau khi hoàn thành ATTACK hoặc HURT
        if ((state == BossState::ATTACK && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == BossState::HURT && sprites[state].currentFrame == sprites[state].clips.size() - 1)) {
            state = BossState::FLYING;
        }
    }

    void attack(std::vector<SDL_Rect>& projectiles) {
        if (state == BossState::DEATH) return;

        Uint32 currentTime = SDL_GetTicks();
        float attackInterval = (hp > 100) ? 2000 : 1000; // Tấn công nhanh hơn khi HP dưới 50%
        if (currentTime - lastAttackTime >= attackInterval) {
            state = BossState::ATTACK;
            SDL_Rect projectile = {static_cast<int>(x), static_cast<int>(y + 20), 32, 32};
            projectiles.push_back(projectile);
            lastAttackTime = currentTime;
        }
    }

    void render(Graphics& graphics) {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 2.f);
    }

    void renderProjectile(Graphics& graphics, const SDL_Rect& projectile) {
        graphics.renderSprite(projectile.x, projectile.y, fireballSprite, true, 1.0f);
    }

    int getHP() const { return hp; }
    void takeDamage(int damage) {
        if (state == BossState::DEATH) return;
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            state = BossState::DEATH;
        } else {
            state = BossState::HURT;
        }
    }
    float getX() const { return x; }
    float getY() const { return y; }
    BossState getState() const { return state; }
    int getAttackDamage() const { return attackDamage; }
};

#endif
