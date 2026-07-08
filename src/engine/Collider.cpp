#include "engine/Collider.h"

#include <cmath>
#include <algorithm>

bool AABB::intersects(const AABB& other) const
{
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}

bool AABB::containsPoint(const glm::vec3& point) const
{
    return (point.x >= min.x && point.x <= max.x) &&
           (point.y >= min.y && point.y <= max.y) &&
           (point.z >= min.z && point.z <= max.z);
}

glm::vec3 AABB::getCenter() const
{
    return (min + max) * 0.5f;
}

glm::vec3 AABB::getHalfSize() const
{
    return (max - min) * 0.5f;
}

AABB AABB::fromCenterSize(const glm::vec3& center, const glm::vec3& halfSize)
{
    return AABB{ center - halfSize, center + halfSize };
}

glm::vec3 AABB::getOverlap(const AABB& other) const
{
    if (!intersects(other))
        return glm::vec3(0.0f);

    float overlapX = std::min(max.x - other.min.x, other.max.x - min.x);
    float overlapY = std::min(max.y - other.min.y, other.max.y - min.y);
    float overlapZ = std::min(max.z - other.min.z, other.max.z - min.z);

    glm::vec3 centerA = getCenter();
    glm::vec3 centerB = other.getCenter();
    glm::vec3 dir     = centerA - centerB;

    if (overlapX <= overlapY && overlapX <= overlapZ) {
        return glm::vec3((dir.x >= 0.0f ? overlapX : -overlapX), 0.0f, 0.0f);
    } else if (overlapY <= overlapX && overlapY <= overlapZ) {
        return glm::vec3(0.0f, (dir.y >= 0.0f ? overlapY : -overlapY), 0.0f);
    } else {
        return glm::vec3(0.0f, 0.0f, (dir.z >= 0.0f ? overlapZ : -overlapZ));
    }
}
