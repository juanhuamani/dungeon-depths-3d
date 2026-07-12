#pragma once

#include "engine/GameObject.h"
#include <glm/glm.hpp>

class Projectile : public GameObject {
public:
    void init(const glm::vec3& startPos, const glm::vec3& direction, float speed);
    void update(float deltaTime) override;

    void reflectX()    { m_direction.x = -m_direction.x; m_bounceCount++; }
    void reflectZ()    { m_direction.z = -m_direction.z; m_bounceCount++; }
    void reflectBoth() { m_direction.x = -m_direction.x; m_direction.z = -m_direction.z; m_bounceCount++; }

    bool  isExpired() const;
    int   getDamage() const  { return m_damage; }
    float getSpeed() const   { return m_speed; }

    static constexpr glm::vec3 HALF_SIZE{0.1f, 0.1f, 0.1f};

private:
    glm::vec3 m_direction{0.0f};
    float     m_speed     = 12.0f;
    float     m_age       = 0.0f;
    int       m_damage    = 1;
    int       m_bounceCount = 0;

    static constexpr int    MAX_BOUNCES  = 3;
    static constexpr float  MAX_LIFETIME = 3.0f;
};
