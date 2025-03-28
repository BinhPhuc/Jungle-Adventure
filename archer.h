#ifndef _ARCHER__H
#define _ARCHER__H

#include "player.h"
#include "graphics.h"
#include <vector>

class Archer : public Player {
private:
    float speed = 5.0f; // Tốc độ di chuyển
    float vx = 0.0f;    // Vận tốc theo trục x
    bool isJumping = false;
    float jumpVelocity = -15.0f; // Vận tốc nhảy ban đầu
    float gravity = 0.5f;       // Trọng lực
    int attackDamage = 15;      // Sát thương mỗi mũi tên
    Uint32 lastShotTime = 0;    // Thời gian bắn mũi tên cuối cùng
    Uint32 shotCooldown = 300;  // Thời gian chờ giữa các lần bắn (ms)

    // Cơ chế ROLL
    bool isRolling = false;
    float rollSpeed = 10.0f;
    float rollDistance = 150.0f; // Tổng khoảng cách lăn
    float currentRollDistance = 0.0f; // Khoảng cách đã lăn trong lần roll hiện tại
    Uint32 lastRollTime = 0;
    Uint32 rollCooldown = 1500;

    // Danh sách mũi tên (projectiles)
    std::vector<Projectile> projectiles;

    // Texture cho mũi tên, chỉ khởi tạo một lần
    SDL_Texture* arrowTexture = nullptr;

    bool hasShield = false;
    Uint32 shieldTimer = 0;

    bool shouldShoot = false;

public:
    void init(Graphics& graphics) override {
        hp = 100; // HP thấp hơn Warrior (Warrior có 120)
        x = 100;
        y = 550; // Mặt đất mặc định
        state = PlayerState::IDLE;

        // Khởi tạo sprite cho các trạng thái
        SDL_Texture* idleTex = graphics.loadTexture("assets/sprites/archer/IDLE.png");
        sprites[PlayerState::IDLE].initAuto(idleTex, 64, 64, 4);

        SDL_Texture* walkTex = graphics.loadTexture("assets/sprites/archer/WALK.png");
        sprites[PlayerState::WALK].initAuto(walkTex, 64, 64, 8);

        SDL_Texture* rollTex = graphics.loadTexture("assets/sprites/archer/ROLL.png");
        sprites[PlayerState::DEFEND].initAuto(rollTex, 64, 64, 7); // Dùng DEFEND để lưu ROLL

        SDL_Texture* jumpTex = graphics.loadTexture("assets/sprites/archer/JUMP.png");
        sprites[PlayerState::JUMP].initAuto(jumpTex, 64, 64, 2);

        SDL_Texture* attackTex = graphics.loadTexture("assets/sprites/archer/ATTACK1.png");
        sprites[PlayerState::ATTACK1].initAuto(attackTex, 64, 64, 7);

        SDL_Texture* deathTex = graphics.loadTexture("assets/sprites/archer/DEATH.png");
        sprites[PlayerState::DEATH].initAuto(deathTex, 64, 64, 8);

        // Tùy chỉnh tốc độ animation
        for (auto& pair : sprites) {
            pair.second.frameDelay = 100; // 100ms mỗi frame
        }
        sprites[PlayerState::ATTACK1].frameDelay = 150; // Tấn công chậm hơn
        sprites[PlayerState::DEFEND].frameDelay = 120;  // ROLL hơi nhanh hơn chút
        sprites[PlayerState::DEATH].frameDelay = 200;   // Chết chậm hơn

        // Khởi tạo texture cho mũi tên
        arrowTexture = graphics.loadTexture("assets/sprites/archer/ARROW.png");
    }

    void handleEvent(const SDL_Event& e) override {
        if (state == PlayerState::DEATH || isRolling) return; // Không xử lý khi chết hoặc đang lăn

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_a:
                    vx = -speed;
                    facingLeft = true;
                    if (!isJumping && state != PlayerState::ATTACK1) {
                        setState(PlayerState::WALK);
                    }
                    break;
                case SDLK_d:
                    vx = speed;
                    facingLeft = false;
                    if (!isJumping && state != PlayerState::ATTACK1) {
                        setState(PlayerState::WALK);
                    }
                    break;
                case SDLK_SPACE:
                    if (!isJumping) {
                        jumpVelocity = -15.0f;
                        isJumping = true;
                        setState(PlayerState::JUMP);
                    }
                    break;
                case SDLK_j: // Tấn công bằng mũi tên
                    if (state != PlayerState::ATTACK1 && SDL_GetTicks() - lastShotTime >= shotCooldown) {
                        state = PlayerState::ATTACK1;
                        sprites[PlayerState::ATTACK1].currentFrame = 0;
                        shouldShoot = true; // Đặt cờ để bắn mũi tên sau
                        lastShotTime = SDL_GetTicks();
                    }
                    break;
                case SDLK_l: // ROLL để né đòn
                    if (SDL_GetTicks() - lastRollTime >= rollCooldown && state != PlayerState::ATTACK1) {
                        setState(PlayerState::DEFEND);
                        isRolling = true;
                        currentRollDistance = 0.0f; // Reset khoảng cách đã lăn
                        lastRollTime = SDL_GetTicks();
                        sprites[PlayerState::DEFEND].currentFrame = 0; // Đặt lại frame animation
                    }
                    break;
            }
        } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
                case SDLK_a:
                case SDLK_d:
                    vx = 0.0f;
                    if (!isJumping && state != PlayerState::ATTACK1 && state != PlayerState::DEFEND) {
                        state = PlayerState::IDLE;
                    }
                    break;
            }
        }
    }

    void update() override {
        if (state == PlayerState::DEATH) {
            sprites[state].tickTimed(SDL_GetTicks());
            return;
        }

        x += vx;
        if (x < 0) x = 0;
        if (x > SCREEN_WIDTH - 64) x = SCREEN_WIDTH - 64;

        if (isJumping) {
            y += jumpVelocity;
            jumpVelocity += gravity;
            if (y >= 550) {
                y = 550;
                isJumping = false;
                if (state == PlayerState::JUMP) state = PlayerState::IDLE;
            }
        }

        if (isRolling) {
            float rollStep = rollSpeed;
            if (facingLeft) x -= rollStep;
            else x += rollStep;
            currentRollDistance += rollStep;

            // Kết thúc roll khi đã lăn đủ khoảng cách hoặc hết animation
            if (currentRollDistance >= rollDistance ||
                sprites[PlayerState::DEFEND].currentFrame == sprites[PlayerState::DEFEND].clips.size() - 1) {
                isRolling = false;
                currentRollDistance = 0.0f; // Reset khoảng cách đã lăn
                if (!isJumping) state = PlayerState::IDLE;
            }
        }

        if (state == PlayerState::ATTACK1 && shouldShoot && sprites[PlayerState::ATTACK1].currentFrame == 6) { // Bắn ở frame 2 (giữa animation)
            Projectile proj(x + (facingLeft ? -40 : 64), y + 20, facingLeft);
            proj.sprite.initAuto(arrowTexture, 40, 5, 1);
            projectiles.push_back(proj);
            shouldShoot = false; // Reset cờ sau khi bắn
        }

        for (auto it = projectiles.begin(); it != projectiles.end();) {
            if (!it->active) {
                it = projectiles.erase(it);
            } else {
                float oldX = it->x;
                it->x += it->facingLeft ? -15.0f : 15.0f;
                if (it->x < 0 || it->x > SCREEN_WIDTH) {
                    it->active = false;
                }
                it->sprite.tickTimed(SDL_GetTicks());
                ++it;
            }
        }

        if (state == PlayerState::ATTACK1 && sprites[PlayerState::ATTACK1].currentFrame == sprites[PlayerState::ATTACK1].clips.size() - 1) {
            if (!isJumping) state = PlayerState::IDLE;
        }
        sprites[state].tickTimed(SDL_GetTicks());
    }

    void render(Graphics& graphics, int offsetX = 0, int offsetY = 0) override {
        graphics.renderSprite(static_cast<int>(x), static_cast<int>(y), sprites[state], facingLeft, 3.0f, offsetX, offsetY);
        // Vẽ mũi tên
        for (const auto& proj : projectiles) {
            if (proj.active) {
                Sprite sprite = proj.sprite;
                graphics.renderSprite(static_cast<int>(proj.x), static_cast<int>(proj.y), sprite, proj.facingLeft, 2.5f, offsetX, offsetY);
            }
        }
    }

    void takeDamage(int damage) override {
        if (state == PlayerState::DEATH || isRolling) return; // Không nhận sát thương khi chết hoặc đang ROLL
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            state = PlayerState::DEATH;
        }
    }

    // Các phương thức ảo từ Player
    std::vector<Projectile>& getProjectiles() override { return projectiles; }
    const std::vector<Projectile>& getProjectiles() const override { return projectiles; }
    void deactivateProjectile(std::vector<Projectile>::iterator it) override {
        if (it != projectiles.end()) it->active = false;
    }
    void setAttackDamage(int newDamage) { attackDamage = newDamage; }

    void setSpeed(float s) { speed = s; }
    float getSpeed() const { return speed; }

    void setShield(bool active) {
        hasShield = active;
        if (active) shieldTimer = SDL_GetTicks();
    }
    void setState(PlayerState newState) {
        if (state != newState) {
            state = newState;
            sprites[state].currentFrame = 0; // Đặt lại frame về 0 khi chuyển trạng thái
        }
    }
    float& getJumpVelocity() { return jumpVelocity; }
    bool& getIsJumping() { return isJumping; }

    ~Archer() {
        projectiles.clear();
        if (arrowTexture) {
            SDL_DestroyTexture(arrowTexture);
            arrowTexture = nullptr;
        }
    }
};

#endif // _ARCHER__H
