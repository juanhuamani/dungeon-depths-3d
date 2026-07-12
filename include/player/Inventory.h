#pragma once

class Inventory {
public:
    int  getHealthPotions() const { return m_healthPotions; }
    void addHealthPotion()        { m_healthPotions++; m_discoveredHealth = true; }
    bool useHealthPotion()        { if (m_healthPotions > 0) { m_healthPotions--; return true; } return false; }

    int  getEnergyPotions() const { return m_energyPotions; }
    void addEnergyPotion()        { m_energyPotions++; m_discoveredEnergy = true; }
    bool useEnergyPotion()        { if (m_energyPotions > 0) { m_energyPotions--; return true; } return false; }

    bool hasDiscoveredHealth() const { return m_discoveredHealth; }
    bool hasDiscoveredEnergy() const { return m_discoveredEnergy; }

    void resetForNewLevel() {}
    void resetAll()              { m_healthPotions = 0; m_energyPotions = 0;
                                   m_discoveredHealth = false; m_discoveredEnergy = false; }

private:
    int  m_healthPotions = 0;
    int  m_energyPotions = 0;
    bool m_discoveredHealth = false;
    bool m_discoveredEnergy = false;
};
