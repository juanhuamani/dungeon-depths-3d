#include "player/ItemManager.h"
#include "player/Player.h"
#include "world/Dungeon.h"
#include "world/Room.h"
#include <cstdlib>

void ItemManager::spawnItem(const glm::vec3& worldPos, ItemType containedItem)
{
    Item item;
    item.init(worldPos, ItemType::TREASURE_CHEST, containedItem);
    m_items.push_back(std::move(item));
}

void ItemManager::spawnFromDungeon(const world::Dungeon& dungeon, float tileSize)
{
    m_items.clear();

    for (const auto& room : dungeon.rooms())
    {
        if (room.isEntrance())
            continue;

        world::TileRect b = room.bounds();
        float cx = (static_cast<float>(b.col) + static_cast<float>(b.width) * 0.5f) * tileSize;
        float cz = (static_cast<float>(b.row) + static_cast<float>(b.height) * 0.5f) * tileSize;

        glm::vec3 corner(
            cx + (static_cast<float>(b.width) * 0.5f - 1.2f) * tileSize,
            0.3f,
            cz + (static_cast<float>(b.height) * 0.5f - 1.2f) * tileSize
        );

        ItemType content = ItemType::HEALTH_POTION;
        if (room.type() == world::RoomType::Treasure) {
            content = ItemType::ENERGY_POTION;
        }

        spawnItem(corner, content);
    }
    
    if (!m_items.empty()) {
        m_items.back().setContainedItem(ItemType::GOLDEN_KEY);
    }
}

void ItemManager::spawnDrop(const glm::vec3& position)
{
    // 50% de probabilidad de no soltar nada
    if ((std::rand() % 100) < 50) return;

    ItemType dropType = ((std::rand() % 100) < 50) ? ItemType::HEALTH_POTION : ItemType::ENERGY_POTION;
    
    // Lo soltamos como un cofre para reutilizar la interacción del jugador
    spawnItem(position, dropType);
}

Item* ItemManager::findChestNear(const glm::vec3& position, float range)
{
    for (auto& item : m_items)
    {
        if (!item.isActive())
            continue;
        float dist = glm::distance(position, item.getPosition());
        if (dist <= range)
            return &item;
    }
    return nullptr;
}

void ItemManager::openChest(Player& player, Item* chest)
{
    if (!chest || !chest->isActive())
        return;

    ItemType reward = chest->getContainedItem();
    if (reward == ItemType::HEALTH_POTION)
        player.getInventory().addHealthPotion();
    else if (reward == ItemType::ENERGY_POTION)
        player.getInventory().addEnergyPotion();
    else if (reward == ItemType::GOLDEN_KEY)
        player.getInventory().addGoldenKey();

    chest->setActive(false);
}

void ItemManager::resetForNewLevel()
{
    m_items.clear();
}
