#include "enemies/ShooterBehavior.h"
#include "enemies/Enemy.h"
#include "enemies/EnemyProjectileManager.h"
#include "player/Player.h"
#include <cstdlib>

void ShooterBehavior::update(Enemy& enemy, float deltaTime) {
    Player* player = enemy.getPlayer();
    if (!player || !player->isAlive()) return;

    if (m_damageTimer > 0.0f) {
        m_damageTimer -= deltaTime;
    }
    
    glm::vec3 toPlayer = player->transform.position - enemy.transform.position;
    toPlayer.y = 0.0f;
    float dist = glm::length(toPlayer);

    // Movimiento aleatorio
    m_moveTimer -= deltaTime;
    if (m_moveTimer <= 0.0f) {
        m_moveTimer = 1.0f + (std::rand() % 200) / 100.0f; // 1 a 3 segundos
        float angle = (std::rand() % 360) * 3.14159f / 180.0f;
        m_moveDirection = glm::vec3(std::cos(angle), 0.0f, std::sin(angle));
        
        // Si está muy lejos, tiende a acercarse un poco
        if (dist > 8.0f) {
            m_moveDirection = glm::normalize(m_moveDirection + glm::normalize(toPlayer));
        }
    }

    enemy.moveWithCollision(m_moveDirection * (enemy.getSpeed() * 0.7f), deltaTime);

    // Disparo
    m_shootTimer -= deltaTime;
    if (m_shootTimer <= 0.0f && dist < 12.0f) { // Solo dispara si está a cierta distancia
        m_shootTimer = 2.0f + (std::rand() % 100) / 100.0f; // 2 a 3 segundos
        
        if (enemy.getProjectileManager()) {
            glm::vec3 shootOrigin = enemy.transform.position + glm::vec3(0.0f, 0.6f, 0.0f);
            enemy.getProjectileManager()->shoot(shootOrigin, toPlayer);
        }
    }

    // Colisión por si acaso el jugador se le acerca mucho
    if (m_damageTimer <= 0.0f && enemy.collider.intersects(player->collider)) {
        player->takeDamage(enemy.getContactDamage());
        m_damageTimer = 1.0f;
    }
}
