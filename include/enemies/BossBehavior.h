#pragma once

#include "enemies/IEnemyBehavior.h"
#include <glm/glm.hpp>

class BossBehavior : public IEnemyBehavior {
public:
    void update(Enemy& enemy, float deltaTime) override;

private:
    float m_shootTimer = 1.5f;
    float m_damageTimer = 0.0f;
    float m_phaseTimer = 5.0f;
    int m_phase = 0;
};
