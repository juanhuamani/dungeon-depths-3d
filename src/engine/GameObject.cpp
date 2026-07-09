#include "engine/GameObject.h"

void GameObject::syncCollider(const glm::vec3& halfSize)
{
    collider = AABB::fromCenterSize(transform.position, halfSize);
}
