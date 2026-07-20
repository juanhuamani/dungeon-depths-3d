#include "enemies/EnemyProjectileManager.h"
#include <algorithm>
#include <cmath>

void EnemyProjectileManager::shoot(const glm::vec3& origin, const glm::vec3& direction) {
    auto it = std::find_if(m_pool.begin(), m_pool.end(),
        [](const Projectile& p) { return !p.active; });

    if (it != m_pool.end()) {
        glm::vec3 dir = glm::length(direction) > 0.001f ? glm::normalize(direction) : glm::vec3(1.0f, 0.0f, 0.0f);
        it->init(origin, dir, PROJECTILE_SPEED);
        // Desactivamos el rebote forzándolo a su estado límite o ignorándolo
        // Como Projectile gestiona sus propios rebotes, lo dejamos normal por simplicidad
    }
}

void EnemyProjectileManager::update(float deltaTime) {
    for (auto& proj : m_pool) {
        if (!proj.active) continue;

        proj.update(deltaTime);

        if (!proj.active) continue;

        // 1. Colisión con el jugador
        if (m_playerCheck && m_playerCheck(proj.collider)) {
            proj.active = false;
            continue;
        }

        // 2. Colisión con paredes
        if (m_wallCheck) {
            glm::vec3 normal(0.0f);
            glm::vec3 resolved(0.0f);
            if (m_wallCheck(proj.collider, normal, resolved)) {
                proj.transform.position = resolved;

                if (std::abs(normal.x) > 0.5f && std::abs(normal.z) > 0.5f)
                    proj.reflectBoth();
                else if (std::abs(normal.x) > 0.5f)
                    proj.reflectX();
                else if (std::abs(normal.z) > 0.5f)
                    proj.reflectZ();

                if (proj.isExpired()) {
                    proj.active = false;
                    continue;
                }
                
                proj.syncCollider(Projectile::HALF_SIZE);
            }
        }
    }
}
