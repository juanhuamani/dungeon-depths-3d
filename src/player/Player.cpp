#include "player/Player.h"
#include <algorithm>
#include <cmath>

Player::Player()
{
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(0.7f, 1.8f, 0.7f);
    syncCollider(HALF_SIZE);
}

void Player::update(float deltaTime)
{
    if (m_meleeCooldown > 0.0f)
        m_meleeCooldown -= deltaTime;
    if (m_rangedCooldown > 0.0f)
        m_rangedCooldown -= deltaTime;

    if (m_attackTimer > 0.0f)
    {
        m_attackTimer -= deltaTime;
        if (m_attackTimer <= 0.0f)
            m_currentAttack = AttackType::NONE;
    }

    syncCollider(HALF_SIZE);
}

void Player::takeDamage(int amount)
{
    m_health = std::max(0, m_health - amount);
}

void Player::heal(int amount)
{
    m_health = std::min(m_maxHealth, m_health + amount);
}

bool Player::isAlive() const
{
    return m_health > 0;
}

void Player::move(const glm::vec3& velocity, float deltaTime)
{
    transform.position += velocity * deltaTime;
}

void Player::setYaw(float yaw)
{
    m_yaw = yaw;
    transform.rotation.y = -yaw - 90.0f;
}

bool Player::attackMelee()
{
    if (m_meleeCooldown > 0.0f)
        return false;

    m_currentAttack = AttackType::MELEE;
    m_attackTimer = ATTACK_DURATION;
    m_meleeCooldown = MELEE_COOLDOWN;
    return true;
}

bool Player::attackRanged()
{
    if (m_rangedCooldown > 0.0f)
        return false;

    m_currentAttack = AttackType::RANGED;
    m_attackTimer = ATTACK_DURATION;
    m_rangedCooldown = RANGED_COOLDOWN;
    return true;
}

AABB Player::getMeleeHitbox() const
{
    glm::vec3 forward = getForwardXZ();
    glm::vec3 center = transform.position + forward * MELEE_RANGE * 0.5f;
    center.y += HALF_SIZE.y;

    glm::vec3 halfSize(MELEE_WIDTH * 0.5f, 0.5f, MELEE_RANGE * 0.5f);
    return AABB::fromCenterSize(center, halfSize);
}

void Player::resetForNewLevel(const glm::vec3& spawnPos)
{
    transform.position = spawnPos;
    m_health = m_maxHealth;
    m_currentAttack = AttackType::NONE;
    m_attackTimer = 0.0f;
    m_meleeCooldown = 0.0f;
    m_rangedCooldown = 0.0f;
    m_inventory.resetForNewLevel();
    syncCollider(HALF_SIZE);
}

glm::vec3 Player::getForwardXZ() const
{
    float yawRad = glm::radians(m_yaw);
    return glm::normalize(glm::vec3(cos(yawRad), 0.0f, sin(yawRad)));
}
