#ifndef _WARRIOR__H
#define _WARRIOR__H

#include "player.h"

class Warrior : public Player {
private:
    float speed = 5.0f;
    float vx = 0.0f; // Vận tốc theo trục x
    bool isJumping = false;
    float jumpVelocity = -15.0f;
    float gravity = 0.5f;
    int attackDamage = 10;
    float attackSpeed = 1.0f; // Tấn công mỗi 1 giây
    Uint32 lastAttackTime = 0;
    int attackCombo = 0; // Theo dõi combo tấn công (1 → 2 → 3)
    float groundLevel = 600.0f; // Tọa độ y của mặt đất (điều chỉnh theo background)

public:
    void init(Graphics& graphics) override {
        hp = 150;
        x = 100;
        y = groundLevel; // Đặt y ban đầu để nhân vật chạm đất
        state = PlayerState::IDLE;

        // Tải sprite cho từng hành động từ các file riêng biệt
        SDL_Texture* idleTex = graphics.loadTexture("assets/sprites/warrior/IDLE.png");
        SDL_Texture* walkTex = graphics.loadTexture("assets/sprites/warrior/WALK.png");
        SDL_Texture* runTex = graphics.loadTexture("assets/sprites/warrior/RUN.png");
        SDL_Texture* jumpTex = graphics.loadTexture("assets/sprites/warrior/JUMP.png");
        SDL_Texture* hurtTex = graphics.loadTexture("assets/sprites/warrior/HURT.png");
        SDL_Texture* attack1Tex = graphics.loadTexture("assets/sprites/warrior/ATTACK1.png");
        SDL_Texture* attack2Tex = graphics.loadTexture("assets/sprites/warrior/ATTACK2.png");
        SDL_Texture* attack3Tex = graphics.loadTexture("assets/sprites/warrior/ATTACK3.png");
        SDL_Texture* deathTex = graphics.loadTexture("assets/sprites/warrior/DEATH.png");

        // Khởi tạo sprite cho từng hành động
        sprites[PlayerState::IDLE].initAuto(idleTex, 96, 84, 7);
        sprites[PlayerState::WALK].initAuto(walkTex, 96, 84, 8);
        sprites[PlayerState::RUN].initAuto(runTex, 96, 84, 8);
        sprites[PlayerState::JUMP].initAuto(jumpTex, 96, 84, 5);
        sprites[PlayerState::HURT].initAuto(hurtTex, 96, 84, 4);
        sprites[PlayerState::ATTACK1].initAuto(attack1Tex, 96, 84, 6);
        sprites[PlayerState::ATTACK2].initAuto(attack2Tex, 96, 84, 5);
        sprites[PlayerState::ATTACK3].initAuto(attack3Tex, 96, 84, 6);
        sprites[PlayerState::DEATH].initAuto(deathTex, 96, 84, 12);

        // Điều chỉnh tốc độ animation
        for (auto& pair : sprites) {
            pair.second.frameDelay = 100; // 100ms mỗi frame
        }
        sprites[PlayerState::ATTACK1].frameDelay = 150; // Tấn công chậm hơn
        sprites[PlayerState::ATTACK2].frameDelay = 150;
        sprites[PlayerState::ATTACK3].frameDelay = 150;
        sprites[PlayerState::DEATH].frameDelay = 200; // Chết chậm hơn
    }

    void handleEvent(const SDL_Event& e) override {
        if (state == PlayerState::DEATH) return; // Không xử lý sự kiện nếu đã chết

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_a:
                    vx = -speed; // Đặt vận tốc sang trái
                    facingLeft = true;
                    if (SDL_GetModState() & KMOD_SHIFT) {
                        setState(PlayerState::RUN);
                        vx = -8.0f; // Tăng tốc khi chạy
                    } else {
                        setState(PlayerState::WALK);
                        vx = -5.0f;
                    }
                    break;
                case SDLK_d:
                    vx = speed; // Đặt vận tốc sang phải
                    facingLeft = false;
                    if (SDL_GetModState() & KMOD_SHIFT) {
                        setState(PlayerState::RUN);
                        vx = 8.0f; // Tăng tốc khi chạy
                    } else {
                        setState(PlayerState::WALK);
                        vx = 5.0f;
                    }
                    break;
                case SDLK_SPACE:
                    if (!isJumping) {
                        jumpVelocity = -15.0f;
                        isJumping = true;
                        setState(PlayerState::JUMP);
                    }
                    break;
                case SDLK_j: // Tấn công
                    Uint32 currentTime = SDL_GetTicks();
                    if (currentTime - lastAttackTime >= attackSpeed * 1000) {
                        attackCombo = (attackCombo + 1) % 3; // Chuyển qua ATTACK1 -> ATTACK2 -> ATTACK3
                        if (attackCombo == 0) setState(PlayerState::ATTACK1);
                        else if (attackCombo == 1) setState(PlayerState::ATTACK2);
                        else setState(PlayerState::ATTACK3);
                        lastAttackTime = currentTime;
                    }
                    break;
            }
        } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) {
                vx = 0.0f; // Dừng di chuyển khi thả phím
                if (!isJumping) setState(PlayerState::IDLE);
            }
        }
    }

    void update() override {
        if (state == PlayerState::DEATH) {
            sprites[state].tickTimed(SDL_GetTicks());
            return;
        }

        // Cập nhật tọa độ x dựa trên vận tốc
        x += vx;

        // Cập nhật vị trí khi nhảy
        if (isJumping) {
            y += jumpVelocity;
            jumpVelocity += gravity;
            if (y >= groundLevel) { // Chạm đất
                y = groundLevel;
                isJumping = false;
                if (state == PlayerState::JUMP) setState(PlayerState::IDLE);
            }
        }

        // Giới hạn vị trí của Warrior
        if (x < 0) x = 0;
        if (x > SCREEN_WIDTH - 96) x = SCREEN_WIDTH - 96;

        // Cập nhật trạng thái dựa trên vận tốc (nếu không nhảy và không tấn công)
        if (!isJumping && state != PlayerState::ATTACK1 && state != PlayerState::ATTACK2 && state != PlayerState::ATTACK3 && state != PlayerState::HURT) {
            if (vx != 0) {
                if (std::abs(vx) > 5.0f) {
                    setState(PlayerState::RUN);
                } else {
                    setState(PlayerState::WALK);
                }
            } else {
                setState(PlayerState::IDLE);
            }
        }

        // Cập nhật animation
        Uint32 now = SDL_GetTicks();
        sprites[state].tickTimed(now);

        // Quay lại trạng thái IDLE sau khi hoàn thành ATTACK hoặc HURT
        if ((state == PlayerState::ATTACK1 && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == PlayerState::ATTACK2 && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == PlayerState::ATTACK3 && sprites[state].currentFrame == sprites[state].clips.size() - 1) ||
            (state == PlayerState::HURT && sprites[state].currentFrame == sprites[state].clips.size() - 1)) {
            if (!isJumping) setState(PlayerState::IDLE);
        }
    }

    void render(Graphics& graphics) override {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 1.5f);
    }

    void attack(std::vector<SDL_Rect>& projectiles) override {
        // Không cần bắn đạn, tấn công cận chiến được xử lý trong handleEvent
    }

    int getHP() const override { return hp; }
    void takeDamage(int damage) override {
        if (state == PlayerState::DEATH) return;
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            setState(PlayerState::DEATH);
        } else {
            setState(PlayerState::HURT);
        }
    }
    float getX() const override { return x; }
    float getY() const override { return y; }
    PlayerState getState() const override { return state; }
    int getAttackDamage() const override { return attackDamage; }

private:
    // Hàm helper để đặt trạng thái và đặt lại frame animation
    void setState(PlayerState newState) {
        if (state != newState) {
            state = newState;
            sprites[state].currentFrame = 0; // Đặt lại frame về 0 khi chuyển trạng thái
        }
    }
};

#endif
