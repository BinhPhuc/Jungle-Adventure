#ifndef _DEMONSLIME__H
#define _DEMONSLIME__H

#include "boss.h"

class DemonSlime : public Boss {
private:
    float speed = 3.5f;
    float vx = -2.0f; // Vận tốc ban đầu (di chuyển sang trái)
    Uint32 lastAttackTime = 0;
    float attackSpeed = 2.0f; // Tấn công mỗi 2 giây
    int attackDamage = 20; // Sát thương của boss
    float groundLevel = 310.0f; // Mặt đất (đồng bộ với Warrior)
    float distanceMoved = 0.0f; // Theo dõi khoảng cách di chuyển
    float minDistanceToAttack = 200.0f;
    float chaseRange = 400.0f; // Phạm vi đuổi theo tối đa
    float startX;

public:
    void init(Graphics& graphics, float startX, float startY, int stage, float warriorX) override {
        x = startX; // 1000
        y = startY; // 550
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
            attackSpeed = 1.5f; // Tấn công nhanh hơn
            Uint32 now = SDL_GetTicks();
        }

        // Logic di chuyển đuổi theo Warrior
        float distanceToWarrior = std::abs(warriorX - x);
        if (distanceToWarrior <= chaseRange && state != BossState::ATTACK && state != BossState::HURT) {
            if (warriorX < x) {
                vx = -speed; // Đuổi sang trái
                facingLeft = true;
            } else {
                vx = speed; // Đuổi sang phải
                facingLeft = false;
            }
            x += vx; // Cập nhật vị trí
            state = BossState::FLYING; // Đang di chuyển
        } else if (distanceToWarrior > chaseRange) {
            // Quay lại vị trí ban đầu nếu Warrior quá xa
            if (x > startX + 10) {
                vx = -speed;
                facingLeft = true;
            } else if (x < startX - 10) {
                vx = speed;
                facingLeft = false;
            } else {
                vx = 0;
                state = BossState::IDLE; // Dừng lại khi về gần vị trí gốc
            }
            x += vx;
        }

        // Giới hạn vị trí để không vượt quá màn hình
        if (x < 0) x = 0;
        if (x >= SCREEN_WIDTH - 720) x = SCREEN_WIDTH - 720;

        y = groundLevel; // Giữ trên mặt đất

        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);
        if ((state == BossState::ATTACK && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == BossState::HURT && sprites[state].currentFrame == sprites[state].clips.size() - 1)) {
            state = BossState::FLYING;
        }


    }

    void attack(std::vector<SDL_Rect>& projectiles, float warriorX) override {
        if (state == BossState::DEATH || state == BossState::HURT) return;

        Uint32 currentTime = SDL_GetTicks();
        float distanceToWarrior = std::abs(warriorX - x);

        // Tấn công khi Warrior trong phạm vi minDistanceToAttack
        if (currentTime - lastAttackTime >= attackSpeed * 1000 && distanceToWarrior <= minDistanceToAttack) {
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
