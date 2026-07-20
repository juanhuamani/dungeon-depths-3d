#pragma once

class Enemy;

class IEnemyBehavior {
public:
    virtual ~IEnemyBehavior() = default;
    
    // Se invoca en cada frame
    virtual void update(Enemy& enemy, float deltaTime) = 0;

    // Se invoca si el enemigo recibe daño, choca con el jugador, etc.
    virtual void onDamageTaken(Enemy& enemy, int amount) {}
};
