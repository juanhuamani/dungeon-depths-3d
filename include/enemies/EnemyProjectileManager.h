#pragma once

#include "player/Projectile.h"
#include <array>
#include <functional>

class EnemyProjectileManager {
public:
    static constexpr int MAX_PROJECTILES = 64;
    static constexpr float PROJECTILE_SPEED = 8.0f; // Un poco más lentos que los del jugador

    void shoot(const glm::vec3& origin, const glm::vec3& direction);
    void update(float deltaTime);

    using WallCollisionFn  = std::function<bool(const AABB&, glm::vec3& outNormal, glm::vec3& outResolved)>;
    using PlayerCollisionFn = std::function<bool(const AABB&)>; // Devuelve true si chocó con el jugador

    void setWallCollisionCallback(WallCollisionFn fn)  { m_wallCheck = std::move(fn); }
    void setPlayerCollisionCallback(PlayerCollisionFn fn) { m_playerCheck = std::move(fn); }

    const std::array<Projectile, MAX_PROJECTILES>& getProjectiles() const { return m_pool; }

private:
    std::array<Projectile, MAX_PROJECTILES> m_pool;
    WallCollisionFn  m_wallCheck;
    PlayerCollisionFn m_playerCheck;
};
