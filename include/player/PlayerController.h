#pragma once

#include "player/Player.h"
#include "engine/Camera.h"

class PlayerController {
public:
    PlayerController(Player& player, Camera& camera);

    void handleInput(float deltaTime);
    void handleMouseMovement(float xOffset, float yOffset);

    bool didShootThisFrame() const { return m_shotFired; }
    glm::vec3 getShootDirection() const { return m_shootDir; }

private:
    Player& m_player;
    Camera& m_camera;

    bool m_shotFired = false;
    glm::vec3 m_shootDir{0.0f};

    void handleMovement(float deltaTime);
    void handleAttacks();
};
