#pragma once

#include "enemies/IEnemyBehavior.h"
#include <glm/glm.hpp>

class ChaserBehavior : public IEnemyBehavior {
public:
    void update(Enemy& enemy, float deltaTime) override;
    
private:
    float m_damageTimer = 0.0f; // Para no hacer daño constante en cada frame
};
