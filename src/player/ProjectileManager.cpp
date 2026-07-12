#include "player/ProjectileManager.h"
#include <algorithm>
#include <cmath>

void ProjectileManager::shoot(const glm::vec3& origin, const glm::vec3& direction)
{
    auto it = std::find_if(m_pool.begin(), m_pool.end(),
        [](const Projectile& p) { return !p.active; });

    if (it != m_pool.end())
        it->init(origin, direction, PROJECTILE_SPEED);
}

void ProjectileManager::update(float deltaTime)
{
    for (auto& proj : m_pool)
    {
        if (!proj.active)
            continue;

        proj.update(deltaTime);

        if (!proj.active)
            continue;

        if (proj.transform.position.y < -0.5f)
        {
            proj.active = false;
            continue;
        }

        glm::vec3 normal;
        glm::vec3 resolved;
        if (m_wallCheck && m_wallCheck(proj.collider, normal, resolved))
        {
            proj.transform.position = resolved;

            if (std::abs(normal.x) > 0.5f && std::abs(normal.z) > 0.5f)
                proj.reflectBoth();
            else if (std::abs(normal.x) > 0.5f)
                proj.reflectX();
            else if (std::abs(normal.z) > 0.5f)
                proj.reflectZ();

            if (proj.isExpired())
            {
                proj.active = false;
                continue;
            }

            proj.syncCollider(Projectile::HALF_SIZE);
        }

        if (m_enemyCheck)
        {
            int enemyId = m_enemyCheck(proj.collider);
            if (enemyId >= 0)
                proj.active = false;
        }
    }
}
