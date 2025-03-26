#ifndef _DEMONSLIME__H
#define _DEMONSLIME__H

#include "boss.h"

class DemonSlime : public Boss {
private:
    float speed = 3.0f;
    float vx = -2.0f; // Vận tốc ban đầu (di chuyển sang trái)
    Uint32 lastAttackTime = 0;
    float attackSpeed = 2.0f; // Tấn công mỗi 2 giây
    int attackDamage = 20; // Sát thương của boss
    float groundLevel = 310.0f; // Mặt đất (đồng bộ với Warrior)
    float distanceMoved = 0.0f; // Theo dõi khoảng cách di chuyển
    float minDistanceToAttack = 200.0f;

public:
    void init(Graphics& graphics, float startX, float startY, int stage) override {
        x = startX;
        y = startY;
        hp = 250; // HP cao hơn boss stage 1
        state = BossState::IDLE;

        // Tải sprite sheet riêng biệt cho từng trạng thái
        SDL_Texture* idleTex = graphics.loadTexture("assets/sprites/demon_slime/IDLE.png");
        SDL_Texture* walkTex = graphics.loadTexture("assets/sprites/demon_slime/WALK.png");
        SDL_Texture* attackTex = graphics.loadTexture("assets/sprites/demon_slime/ATTACK.png");
        SDL_Texture* hurtTex = graphics.loadTexture("assets/sprites/demon_slime/HURT.png");
        SDL_Texture* deathTex = graphics.loadTexture("assets/sprites/demon_slime/DEATH.png");

        // Khởi tạo sprite cho từng trạng thái bằng initAuto
        sprites[BossState::IDLE].initAuto(idleTex, 288, 160, 6);
        sprites[BossState::FLYING].initAuto(walkTex, 288, 160, 12); // Dùng FLYING làm WALK
        sprites[BossState::ATTACK].initAuto(attackTex, 288, 160, 14);
        sprites[BossState::HURT].initAuto(hurtTex, 288, 160, 5);
        sprites[BossState::DEATH].initAuto(deathTex, 288, 160, 14);

        // Điều chỉnh tốc độ animation
        for (auto& pair : sprites) {
            pair.second.frameDelay = 100; // 100ms mỗi frame
        }
        sprites[BossState::ATTACK].frameDelay = 150; // Tấn công chậm hơn
        sprites[BossState::DEATH].frameDelay = 200;  // Chết chậm hơn
    }

    void update() override {
        if (state == BossState::DEATH) {
            sprites[state].tickTimed(SDL_GetTicks());
            return;
        }

        // Chỉ di chuyển nếu không ở trạng thái ATTACK hoặc HURT
        if (state != BossState::ATTACK && state != BossState::HURT) {
            float oldX = x; // Lưu vị trí cũ
            x += vx;
            distanceMoved += std::abs(x - oldX); // Tính khoảng cách di chuyển
            const int spriteWidth = 720;
            if (x <= 0) {
                x = 0;
                vx = speed;
                facingLeft = true;
                state = BossState::FLYING;
            } else if (x >= SCREEN_WIDTH - spriteWidth) {
                x = SCREEN_WIDTH - spriteWidth;
                vx = -speed;
                facingLeft = false;
                state = BossState::FLYING;
            }
        }

        y = groundLevel;

        // Chỉ quay lại FLYING khi animation ATTACK hoặc HURT hoàn thành
        if ((state == BossState::ATTACK && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == BossState::HURT && sprites[state].currentFrame == sprites[state].clips.size() - 1)) {
            state = BossState::FLYING;
        }

        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);
    }

    void attack(std::vector<SDL_Rect>& projectiles) override {
        if (state == BossState::DEATH || state == BossState::HURT) return;

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastAttackTime >= attackSpeed * 1000 && distanceMoved >= minDistanceToAttack) {
            state = BossState::ATTACK;
            sprites[state].currentFrame = 0; // Reset frame về 0 để animation bắt đầu lại
            lastAttackTime = currentTime;
            distanceMoved = 0.0f;
        }
    }

    // Ghi đè hàm renderProjectile (vì Demon Slime không bắn đạn)
    void renderProjectile(Graphics& graphics, const SDL_Rect& projectile) override {
        // Không làm gì, vì Demon Slime không có đạn
    }

    void render(Graphics& graphics) override {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 2.5f);
    }

    int getAttackDamage() const override { return attackDamage; }
};

#endif
