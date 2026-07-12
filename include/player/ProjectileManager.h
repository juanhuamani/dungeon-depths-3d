#pragma once

#include "player/Projectile.h"
#include <array>
#include <functional>

class ProjectileManager {
public:
    static constexpr int MAX_PROJECTILES = 32;
    static constexpr float PROJECTILE_SPEED = 12.0f;

    void shoot(const glm::vec3& origin, const glm::vec3& direction);
    void update(float deltaTime);

    using WallCollisionFn  = std::function<bool(const AABB&, glm::vec3& outNormal, glm::vec3& outResolved)>;
    using EnemyCollisionFn = std::function<int(const AABB&)>;

    void setWallCollisionCallback(WallCollisionFn fn)  { m_wallCheck = std::move(fn); }
    void setEnemyCollisionCallback(EnemyCollisionFn fn) { m_enemyCheck = std::move(fn); }

    const std::array<Projectile, MAX_PROJECTILES>& getProjectiles() const { return m_pool; }

private:
    std::array<Projectile, MAX_PROJECTILES> m_pool;
    WallCollisionFn  m_wallCheck;
    EnemyCollisionFn m_enemyCheck;
};
