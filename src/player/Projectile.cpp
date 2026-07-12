#include "player/Projectile.h"

void Projectile::init(const glm::vec3& startPos, const glm::vec3& direction, float speed)
{
    transform.position = startPos;
    transform.scale = HALF_SIZE * 2.0f;
    m_direction = glm::normalize(direction);
    m_speed = speed;
    m_age = 0.0f;
    m_bounceCount = 0;
    active = true;
    syncCollider(HALF_SIZE);
}

void Projectile::update(float deltaTime)
{
    if (!active)
        return;

    m_age += deltaTime;
    transform.position += m_direction * m_speed * deltaTime;
    syncCollider(HALF_SIZE);

    if (isExpired())
        active = false;
}

bool Projectile::isExpired() const
{
    return m_age >= MAX_LIFETIME || m_bounceCount >= MAX_BOUNCES;
}
