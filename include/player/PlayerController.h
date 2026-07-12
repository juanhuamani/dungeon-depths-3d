#pragma once

#include "player/Player.h"
#include "player/ProjectileManager.h"
#include "engine/Camera.h"

class PlayerController {
public:
    PlayerController(Player& player, Camera& camera, ProjectileManager& projectileManager);

    void handleInput(float deltaTime);
    void handleMouseMovement(float xOffset, float yOffset);

    bool didShootThisFrame() const { return m_shotFired; }

private:
    Player&            m_player;
    Camera&            m_camera;
    ProjectileManager& m_projectiles;

    bool m_shotFired = false;

    void handleMovement(float deltaTime);
    void handleAttacks();
};
