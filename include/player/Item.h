#pragma once

#include "engine/ItemType.h"
#include "engine/Collider.h"
#include <glm/glm.hpp>

class Item {
public:
    void init(const glm::vec3& position, ItemType type, ItemType containedItem = ItemType::HEALTH_POTION);

    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    const glm::vec3& getPosition() const { return m_position; }
    ItemType getType() const { return m_type; }
    ItemType getContainedItem() const { return m_containedItem; }
    void setContainedItem(ItemType type) { m_containedItem = type; }
    const AABB& getCollider() const { return m_collider; }

    static constexpr glm::vec3 HALF_SIZE{0.3f, 0.3f, 0.3f};

private:
    glm::vec3 m_position{0.0f};
    ItemType  m_type = ItemType::TREASURE_CHEST;
    ItemType  m_containedItem = ItemType::HEALTH_POTION;
    bool      m_active = false;
    AABB      m_collider{};

    void syncCollider();
};
