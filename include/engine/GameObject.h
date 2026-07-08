#pragma once

#include "engine/Transform.h"
#include "engine/Collider.h"

class GameObject {
public:
    Transform transform;
    AABB      collider;
    bool      active = true;

    virtual ~GameObject() = default;

    virtual void update(float deltaTime) = 0;

    void syncCollider(const glm::vec3& halfSize);
};
