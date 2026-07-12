#include "player/Item.h"

void Item::init(const glm::vec3& position, ItemType type, ItemType containedItem)
{
    m_position = position;
    m_type = type;
    m_containedItem = containedItem;
    m_active = true;
    syncCollider();
}

void Item::syncCollider()
{
    m_collider = AABB::fromCenterSize(m_position, HALF_SIZE);
}
