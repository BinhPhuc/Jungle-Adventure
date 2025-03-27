#ifndef _DEMONSLIME__H
#define _DEMONSLIME__H

#include "boss.h"

class DemonSlime : public Boss {
private:
    float speed = 3.5f;
    float vx = -2.0f; // Vận tốc ban đầu (di chuyển sang trái)
    Uint32 lastAttackTime = 0;
    float attackSpeed = 2.5f; // Tấn công mỗi 2 giây
    int attackDamage = 15; // Sát thương của boss
    float groundLevel = 310.0f; // Mặt đất (đồng bộ với Warrior)
    float distanceMoved = 0.0f; // Theo dõi khoảng cách di chuyển
    float minDistanceToAttack = 200.0f;
    float chaseRange = 400.0f; // Phạm vi đuổi theo tối đa
    float startX;
    float targetX = 550.0f;

public:
    void init(Graphics& graphics, float startX, float startY, int stage, float warriorX) override {
        x = startX; // 1000
        y = startY; // 550
        this->startX = 640;
        hp = 250;
        state = BossState::FLYING; // Bắt đầu di chuyển ngay

        // Thiết lập hướng ban đầu dựa trên warriorX
        if (warriorX < startX) {
            vx = -speed; // Đi sang trái
            facingLeft = true; // Quay mặt sang trái
        } else {
            vx = speed; // Đi sang phải
            facingLeft = false; // Quay mặt sang phải
        }

        // Tải sprite
        SDL_Texture* idleTex = graphics.loadTexture("assets/sprites/demon_slime/IDLE.png");
        SDL_Texture* walkTex = graphics.loadTexture("assets/sprites/demon_slime/WALK.png");
        SDL_Texture* attackTex = graphics.loadTexture("assets/sprites/demon_slime/ATTACK.png");
        SDL_Texture* hurtTex = graphics.loadTexture("assets/sprites/demon_slime/HURT.png");
        SDL_Texture* deathTex = graphics.loadTexture("assets/sprites/demon_slime/DEATH.png");

        sprites[BossState::IDLE].initAuto(idleTex, 288, 160, 6);
        sprites[BossState::FLYING].initAuto(walkTex, 288, 160, 12);
        sprites[BossState::ATTACK].initAuto(attackTex, 288, 160, 14);
        sprites[BossState::HURT].initAuto(hurtTex, 288, 160, 5);
        sprites[BossState::DEATH].initAuto(deathTex, 288, 160, 14);

        for (auto& pair : sprites) {
            pair.second.frameDelay = 100;
        }
        sprites[BossState::ATTACK].frameDelay = 150;
        sprites[BossState::DEATH].frameDelay = 200;
    }

    bool getFacingLeft() const { return facingLeft; }

    void update(float warriorX) override {
        if (state == BossState::DEATH) {
            sprites[state].tickTimed(SDL_GetTicks());
            return;
        }

        if (hp < 125) {
            speed = 4.5f;
            attackSpeed = 2.0f; // Tấn công nhanh hơn khi HP thấp
        }

        // Logic di chuyển: di chuyển từ x = 1000 đến targetX = 750 và dừng lại
        if (x > targetX) {
            x += vx; // Di chuyển sang trái
            facingLeft = true; // Quay mặt sang trái
            if (state != BossState::ATTACK && state != BossState::HURT) {
                state = BossState::FLYING;
            }
        } else {
            x = targetX; // Dừng lại ở targetX
            vx = 0; // Dừng di chuyển
            if (state != BossState::ATTACK && state != BossState::HURT) {
                state = BossState::IDLE;
            }
        }

        y = groundLevel; // Giữ trên mặt đất

        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);
        if ((state == BossState::ATTACK && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == BossState::HURT && sprites[state].currentFrame == sprites[state].clips.size() - 1)) {
            state = (x > targetX) ? BossState::FLYING : BossState::IDLE;
        }
    }

    void attack(std::vector<SDL_Rect>& projectiles, float warriorX) override {
        if (state == BossState::DEATH) return;

        Uint32 currentTime = SDL_GetTicks();
        // Tấn công liên tục mỗi attackSpeed giây
        if (currentTime - lastAttackTime >= attackSpeed * 1000) {
            state = BossState::ATTACK;
            sprites[state].currentFrame = 0;
            lastAttackTime = currentTime;
        }
    }

    // Ghi đè hàm renderProjectile (vì Demon Slime không bắn đạn)
    void renderProjectile(Graphics& graphics, const SDL_Rect& projectile) override {
        // Không làm gì, vì Demon Slime không có đạn
    }

    void render(Graphics& graphics) override {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], !facingLeft, 2.5f);
    }

    int getAttackDamage() const override { return attackDamage; }
};

#endif
