#pragma once

#include "enemies/IEnemyBehavior.h"
#include <glm/glm.hpp>

class ChargerBehavior : public IEnemyBehavior {
public:
    void update(Enemy& enemy, float deltaTime) override;

private:
    enum class State { WANDERING, WINDUP, CHARGING, RECOVERING };
    State m_state = State::WANDERING;
    
    float m_timer = 2.0f;
    glm::vec3 m_chargeDirection{0.0f};
    float m_damageTimer = 0.0f;
};
