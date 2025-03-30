#ifndef _ARCHER__H
#define _ARCHER__H

#include "player.h"
#include "graphics.h"
#include <vector>

class Archer : public Player {
private:
    float speed = 5.0f;
    float vx = 0.0f;
    bool isJumping = false;
    float jumpVelocity = -15.0f;
    float gravity = 0.5f;
    int attackDamage = 15;
    Uint32 lastShotTime = 0;
    Uint32 shotCooldown = 300;

    bool isRolling = false;
    float rollSpeed = 10.0f;
    float rollDistance = 150.0f;
    float currentRollDistance = 0.0f;
    Uint32 lastRollTime = 0;
    Uint32 rollCooldown = 1000;

    std::vector<Projectile> projectiles;

    SDL_Texture* arrowTexture = nullptr;

    bool hasShield = false;
    Uint32 shieldTimer = 0;

    bool shouldShoot = false;

    float rage = 0.0f;
    Uint32 rageTimer = 0;
    bool rageActive = false;
    Uint32 rageDuration = 5000;
    Uint32 rageEndTime = 0;

public:
    void init(Graphics& graphics) override {
        hp = 100;
        x = 100;
        y = 550;
        state = PlayerState::IDLE;

        SDL_Texture* idleTex = graphics.loadTexture("assets/sprites/archer/IDLE.png");
        sprites[PlayerState::IDLE].initAuto(idleTex, 64, 64, 4);

        SDL_Texture* walkTex = graphics.loadTexture("assets/sprites/archer/WALK.png");
        sprites[PlayerState::WALK].initAuto(walkTex, 64, 64, 8);

        SDL_Texture* rollTex = graphics.loadTexture("assets/sprites/archer/ROLL.png");
        sprites[PlayerState::DEFEND].initAuto(rollTex, 64, 64, 7);

        SDL_Texture* jumpTex = graphics.loadTexture("assets/sprites/archer/JUMP.png");
        sprites[PlayerState::JUMP].initAuto(jumpTex, 64, 64, 2);

        SDL_Texture* attackTex = graphics.loadTexture("assets/sprites/archer/ATTACK1.png");
        sprites[PlayerState::ATTACK1].initAuto(attackTex, 64, 64, 7);

        SDL_Texture* deathTex = graphics.loadTexture("assets/sprites/archer/DEATH.png");
        sprites[PlayerState::DEATH].initAuto(deathTex, 64, 64, 8);

        for (auto& pair : sprites) {
            pair.second.frameDelay = 100;
        }
        sprites[PlayerState::ATTACK1].frameDelay = 150;
        sprites[PlayerState::DEFEND].frameDelay = 120;
        sprites[PlayerState::DEATH].frameDelay = 200;

        arrowTexture = graphics.loadTexture("assets/sprites/archer/ARROW.png");

        rage = 0.0f;
        rageTimer = SDL_GetTicks();
    }

    void handleEvent(const SDL_Event& e) override {
        if (state == PlayerState::DEATH || isRolling) return;

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
                case SDLK_j:
                    if (state != PlayerState::ATTACK1 && SDL_GetTicks() - lastShotTime >= shotCooldown) {
                        state = PlayerState::ATTACK1;
                        sprites[PlayerState::ATTACK1].currentFrame = 0;
                        shouldShoot = true;
                        lastShotTime = SDL_GetTicks();
                    }
                    break;
                case SDLK_i: {
                    if (rage >= 100.0f && !rageActive) {
                        rage = 0.0f;
                        rageTimer = SDL_GetTicks();
                        rageActive = true;
                        rageEndTime = SDL_GetTicks() + rageDuration;
                        shotCooldown = 150;
                        attackDamage += 5;
                        hp += 35;
                        if (hp > 170) hp = 170;
                    }
                    break;
                }
                case SDLK_l:
                    if (SDL_GetTicks() - lastRollTime >= rollCooldown && state != PlayerState::ATTACK1) {
                        setState(PlayerState::DEFEND);
                        isRolling = true;
                        currentRollDistance = 0.0f;
                        lastRollTime = SDL_GetTicks();
                        sprites[PlayerState::DEFEND].currentFrame = 0;
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

        if (rage < 100.0f && !rageActive) {
            Uint32 currentTime = SDL_GetTicks();
            float elapsedTime = (currentTime - rageTimer) / 1000.0f;
            rage = (elapsedTime / 15.0f) * 100.0f;
            if (rage > 100.0f) rage = 100.0f;
        }

        if (rageActive && SDL_GetTicks() > rageEndTime) {
            rageActive = false;
            shotCooldown = 300;
            attackDamage -= 5;
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
            if (currentRollDistance >= rollDistance ||
                sprites[PlayerState::DEFEND].currentFrame == sprites[PlayerState::DEFEND].clips.size() - 1) {
                isRolling = false;
                currentRollDistance = 0.0f;
                if (!isJumping) state = PlayerState::IDLE;
            }
        }

        if (state == PlayerState::ATTACK1 && shouldShoot && sprites[PlayerState::ATTACK1].currentFrame == 6) {
            Projectile proj(x + (facingLeft ? -40 : 64), y + 20, facingLeft);
            proj.sprite.initAuto(arrowTexture, 40, 5, 1);
            projectiles.push_back(proj);
            shouldShoot = false;
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
        for (const auto& proj : projectiles) {
            if (proj.active) {
                Sprite sprite = proj.sprite;
                graphics.renderSprite(static_cast<int>(proj.x), static_cast<int>(proj.y), sprite, proj.facingLeft, 2.5f, offsetX, offsetY);
            }
        }
    }

    void takeDamage(int damage) override {
        if (state == PlayerState::DEATH || isRolling) return;
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            state = PlayerState::DEATH;
        }
    }

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
            sprites[state].currentFrame = 0;
        }
    }
    float& getJumpVelocity() { return jumpVelocity; }
    bool& getIsJumping() { return isJumping; }
    float getRage() const { return rage; }

    ~Archer() {
        projectiles.clear();
        if (arrowTexture) {
            SDL_DestroyTexture(arrowTexture);
            arrowTexture = nullptr;
        }
    }
};

#endif
