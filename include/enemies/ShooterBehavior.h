#pragma once

#include "enemies/IEnemyBehavior.h"
#include <glm/glm.hpp>

class ShooterBehavior : public IEnemyBehavior {
public:
    void update(Enemy& enemy, float deltaTime) override;

private:
    float m_shootTimer = 2.0f;
    float m_moveTimer = 0.0f;
    glm::vec3 m_moveDirection{0.0f};
    float m_damageTimer = 0.0f;
};
