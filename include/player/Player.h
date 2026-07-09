#pragma once

#include "engine/GameObject.h"
#include "player/Inventory.h"
#include <glm/glm.hpp>

enum class AttackType { NONE, MELEE, RANGED };

class Player : public GameObject {
public:
    Player();

    void update(float deltaTime) override;

    void takeDamage(int amount);
    void heal(int amount);
    bool isAlive() const;

    void move(const glm::vec3& velocity, float deltaTime);
    void setYaw(float yaw);

    bool attackMelee();
    bool attackRanged();

    bool isAttacking() const { return m_attackTimer > 0.0f; }
    AttackType getCurrentAttack() const { return m_currentAttack; }

    AABB getMeleeHitbox() const;

    void resetForNewLevel(const glm::vec3& spawnPos);

    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    float getSpeed() const { return m_speed; }
    float getYaw() const { return m_yaw; }
    const Inventory& getInventory() const { return m_inventory; }
    Inventory& getInventory() { return m_inventory; }

    glm::vec3 getForwardXZ() const;

    static constexpr glm::vec3 HALF_SIZE{0.35f, 0.9f, 0.35f};

    static constexpr float MELEE_COOLDOWN = 0.5f;
    static constexpr float RANGED_COOLDOWN = 0.8f;
    static constexpr float MELEE_RANGE = 1.8f;
    static constexpr float MELEE_WIDTH = 1.2f;
    static constexpr float ATTACK_DURATION = 0.3f;
    static constexpr int MELEE_DAMAGE = 2;
    static constexpr int RANGED_DAMAGE = 1;

private:
    int m_health = 5;
    int m_maxHealth = 5;
    float m_speed = 5.0f;
    float m_yaw = -90.0f;
    Inventory m_inventory;

    AttackType m_currentAttack = AttackType::NONE;
    float m_attackTimer = 0.0f;
    float m_meleeCooldown = 0.0f;
    float m_rangedCooldown = 0.0f;
};
