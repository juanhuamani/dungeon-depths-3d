#include "enemies/Enemy.h"
#include "enemies/IEnemyBehavior.h"
#include "game/Game.h"
#include <algorithm>
#include <cmath>

Enemy::Enemy(EnemyType type, const glm::vec3& startPos, int maxHealth, float speed, int contactDamage,
             game::Game* game, Player* player, EnemyProjectileManager* projManager)
    : m_type(type), m_maxHealth(maxHealth), m_health(maxHealth), m_speed(speed), m_contactDamage(contactDamage),
      m_game(game), m_player(player), m_projManager(projManager) 
{
    transform.position = startPos;
    transform.scale = glm::vec3(1.0f);
    syncCollider(HALF_SIZE);
}

Enemy::~Enemy() = default;

void Enemy::update(float deltaTime) {
    if (!isAlive() || !active) return;
    
    if (m_behavior) {
        m_behavior->update(*this, deltaTime);
    }

    // Asegurarse de que el collider se actualiza con la posición
    syncCollider(HALF_SIZE);
}

void Enemy::takeDamage(int amount) {
    if (!isAlive()) return;
    m_health -= amount;
    if (m_behavior) {
        m_behavior->onDamageTaken(*this, amount);
    }
}

void Enemy::setBehavior(std::unique_ptr<IEnemyBehavior> behavior) {
    m_behavior = std::move(behavior);
}

void Enemy::moveWithCollision(const glm::vec3& velocity, float deltaTime) {
    if (glm::length(velocity) > 0.001f) {
        transform.position += velocity * deltaTime;
        
        // Resolver colisiones con paredes
        if (m_game) {
            transform.position = m_game->resolveWallCollision(transform.position, HALF_SIZE);
        }
        syncCollider(HALF_SIZE);
    }
}
