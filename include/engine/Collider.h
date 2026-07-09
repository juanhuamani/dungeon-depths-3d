#pragma once

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min{0.0f};
    glm::vec3 max{0.0f};

    bool intersects(const AABB& other) const;
    bool containsPoint(const glm::vec3& point) const;
    glm::vec3 getCenter() const;
    glm::vec3 getHalfSize() const;
    static AABB fromCenterSize(const glm::vec3& center, const glm::vec3& halfSize);
    glm::vec3 getOverlap(const AABB& other) const;
};
