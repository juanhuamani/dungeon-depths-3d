#include "enemies/ChaserBehavior.h"
#include "enemies/Enemy.h"
#include "player/Player.h"

void ChaserBehavior::update(Enemy& enemy, float deltaTime) {
    Player* player = enemy.getPlayer();
    if (!player || !player->isAlive()) return;

    if (m_damageTimer > 0.0f) {
        m_damageTimer -= deltaTime;
    }

    glm::vec3 direction = player->transform.position - enemy.transform.position;
    direction.y = 0.0f; // Ignorar el eje Y
    
    float dist = glm::length(direction);
    
    // Mover hacia el jugador si está lejos, si está muy cerca se detiene
    if (dist > 0.1f) {
        direction = glm::normalize(direction);
        enemy.moveWithCollision(direction * enemy.getSpeed(), deltaTime);
    }
    
    // Colisión con el jugador (daño por contacto)
    if (m_damageTimer <= 0.0f && enemy.collider.intersects(player->collider)) {
        player->takeDamage(enemy.getContactDamage());
        m_damageTimer = 1.0f; // Cooldown de 1 segundo para no matar instantáneamente
    }
}
