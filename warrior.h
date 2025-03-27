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
    int attackDamage = 5;
    float attackSpeed = 1.0f; // Cooldown giữa các lần nhấn J
    Uint32 lastAttackTime = 0;
    float groundLevel = 550.0f; // Tọa độ y của mặt đất (điều chỉnh theo background)
    int attackCombo = 0;
    bool isInCombo = false;
    bool shouldDealDamage = false;
    bool hasShield = false;
    Uint32 shieldTimer = 0;

    // Thêm biến cho thanh nộ
    float rage = 0.0f; // Giá trị thanh nộ (0-100)
    Uint32 rageTimer = 0; // Thời gian để tính tăng nộ

public:
    void init(Graphics& graphics) override {
        hp = 120;
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
        SDL_Texture* defendTex = graphics.loadTexture("assets/sprites/warrior/DEFEND.png");
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
        sprites[PlayerState::DEFEND].initAuto(defendTex, 96, 84, 6);
        sprites[PlayerState::DEATH].initAuto(deathTex, 96, 84, 12);

        // Điều chỉnh tốc độ animation
        for (auto& pair : sprites) {
            pair.second.frameDelay = 100; // 100ms mỗi frame
        }
        sprites[PlayerState::ATTACK1].frameDelay = 150; // Tấn công chậm hơn
        sprites[PlayerState::ATTACK2].frameDelay = 150; // Tấn công chậm hơn
        sprites[PlayerState::ATTACK3].frameDelay = 150; // Tấn công chậm hơn
        sprites[PlayerState::DEATH].frameDelay = 200; // Chết chậm hơn

        // Khởi tạo thanh nộ
        rage = 0.0f;
        rageTimer = SDL_GetTicks();
    }

    void handleEvent(const SDL_Event& e) override {
        if (state == PlayerState::DEATH) return; // Không xử lý sự kiện nếu đã chết

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_a: {
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
                }
                case SDLK_d: {
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
                }
                case SDLK_SPACE: {
                    if (!isJumping) {
                        jumpVelocity = -15.0f;
                        isJumping = true;
                        setState(PlayerState::JUMP);
                    }
                    break;
                }
                case SDLK_j: { // Tấn công (ATTACK1 -> ATTACK2 -> ATTACK3)
                    Uint32 currentTime = SDL_GetTicks();
                    if (currentTime - lastAttackTime >= attackSpeed * 1000 &&
                        state != PlayerState::ATTACK1 && state != PlayerState::ATTACK2 &&
                        state != PlayerState::ATTACK3 && state != PlayerState::DEFEND) {
                        setState(PlayerState::ATTACK1);
                        attackCombo = 0; // Bắt đầu từ ATTACK1
                        isInCombo = true; // Đánh dấu đang trong chuỗi combo
                        lastAttackTime = currentTime;
                    }
                    break;
                }
                case SDLK_l: { // Phòng thủ (DEFEND)
                    if (state != PlayerState::ATTACK1 && state != PlayerState::ATTACK2 &&
                        state != PlayerState::ATTACK3 && state != PlayerState::DEFEND) {
                        setState(PlayerState::DEFEND);
                        setIsDefending(true);
                        isInCombo = false; // Hủy combo nếu đang trong combo
                    }
                    break;
                }
                case SDLK_i: { // Tiêu hao thanh nộ
                    if (rage >= 100.0f) { // Chỉ khi thanh nộ đầy
                        rage = 0.0f; // Đặt lại thanh nộ
                        rageTimer = SDL_GetTicks(); // Đặt lại thời gian
                        attackDamage += 2; // Tăng sát thương thêm 2
                        hp += 30; // Tăng máu thêm 30
                        if (hp > 150) hp = 150; // Giới hạn máu tối đa
                    }
                    break;
                }
            }
        } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d) {
                vx = 0.0f; // Dừng di chuyển khi thả phím
                if (!isJumping && state != PlayerState::ATTACK1 && state != PlayerState::ATTACK2 &&
                    state != PlayerState::ATTACK3 && state != PlayerState::DEFEND) {
                    setState(PlayerState::IDLE);
                }
            } else if (e.key.keysym.sym == SDLK_l) { // Thoát trạng thái DEFEND
                if (state == PlayerState::DEFEND) {
                    setState(PlayerState::IDLE);
                    setIsDefending(false);
                }
            }
        }
    }

    void update() override {
        if (state == PlayerState::DEATH) {
            sprites[state].tickTimed(SDL_GetTicks());
            return;
        }

        // Cập nhật thanh nộ
        if (rage < 100.0f) {
            Uint32 currentTime = SDL_GetTicks();
            float elapsedTime = (currentTime - rageTimer) / 1000.0f; // Thời gian trôi qua (giây)
            rage = (elapsedTime / 15.0f) * 100.0f; // Tăng 100% trong 10 giây
            if (rage > 100.0f) rage = 100.0f; // Giới hạn tối đa
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
        if (!isJumping && state != PlayerState::ATTACK1 && state != PlayerState::ATTACK2 &&
            state != PlayerState::ATTACK3 && state != PlayerState::DEFEND && state != PlayerState::HURT) {
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

        shouldDealDamage = false;

        if (isInCombo && (state == PlayerState::ATTACK1 || state == PlayerState::ATTACK2 || state == PlayerState::ATTACK3)) {
            // Gây sát thương tại frame cuối của mỗi đòn
            if (sprites[state].currentFrame == sprites[state].clips.size() - 1) {
                shouldDealDamage = true; // Báo hiệu cần gây sát thương

                // Chuyển sang đòn tiếp theo trong combo
                if (state == PlayerState::ATTACK1) {
                    setState(PlayerState::ATTACK2);
                    attackCombo = 1;
                } else if (state == PlayerState::ATTACK2) {
                    setState(PlayerState::ATTACK3);
                    attackCombo = 2;
                } else if (state == PlayerState::ATTACK3) {
                    setState(PlayerState::IDLE);
                    attackCombo = 0;
                    isInCombo = false; // Kết thúc chuỗi combo
                }
            }
        }

        // Quay lại trạng thái IDLE sau khi hoàn thành ATTACK hoặc HURT
        if ((state == PlayerState::DEFEND || state == PlayerState::HURT) &&
            sprites[state].currentFrame == sprites[state].clips.size() - 1) {
            if (state == PlayerState::DEFEND) {
                setIsDefending(false);
            }
            if (!isJumping) setState(PlayerState::IDLE);
        }
    }

    bool getShouldDealDamage() const { return shouldDealDamage; }

    void render(Graphics& graphics) override {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 2.5f);
    }

    void attack(std::vector<SDL_Rect>& projectiles) override {
        // Không cần bắn đạn, tấn công cận chiến được xử lý trong handleEvent
    }

    void setSpeed(float s) { speed = s; }
    float getSpeed() const { return speed; }

    void setShield(bool active) {
        hasShield = active;
        if (active) shieldTimer = SDL_GetTicks();
    }
    void takeDamage(int damage) override {
        if (state == PlayerState::DEATH) return;
        if (!isDefending) {
            int finalDamage = damage;
            if (hasShield && SDL_GetTicks() - shieldTimer < 10000) { // Shield kéo dài 10 giây
                finalDamage = static_cast<int>(damage * 0.8f); // Giảm 20% sát thương
            }
            hp -= finalDamage;
            if (hp <= 0) {
                hp = 0;
                setState(PlayerState::DEATH);
            } else {
                setState(PlayerState::HURT);
            }
            isInCombo = false;
        }
    }
    float getX() const override { return x; }
    float getY() const override { return y; }
    void setY(float newY) override { y = newY; }
    int getHP() const override { return hp; }
    void setHP(int newHP) override { hp = newHP; }
    int getAttackDamage() const override { return attackDamage; }
    void setAttackDamage(int newDamage) { attackDamage = newDamage; }

    PlayerState getState() const override { return state; }
    float& getJumpVelocity() { return jumpVelocity; }
    bool& getIsJumping() { return isJumping; }
    // Thêm hàm để lấy giá trị thanh nộ
    float getRage() const { return rage; }
    void setState(PlayerState newState) {
        if (state != newState) {
            state = newState;
            sprites[state].currentFrame = 0; // Đặt lại frame về 0 khi chuyển trạng thái
        }
    }
};

#endif
