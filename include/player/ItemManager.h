#pragma once

#include "player/Item.h"
#include <vector>

namespace world { class Dungeon; }

class Player;

class ItemManager {
public:
    void spawnFromDungeon(const world::Dungeon& dungeon, float tileSize);
    void spawnDrop(const glm::vec3& position);
    Item* findChestNear(const glm::vec3& position, float range);
    void openChest(Player& player, Item* chest);
    void resetForNewLevel();

    const std::vector<Item>& getItems() const { return m_items; }
    std::vector<Item>& getItems() { return m_items; }

private:
    void spawnItem(const glm::vec3& worldPos, ItemType containedItem);
    std::vector<Item> m_items;
};
