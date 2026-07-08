#pragma once

class Inventory {
public:
    void addTreasureKey()        { m_treasureKeys++; }
    bool useTreasureKey()        { if (m_treasureKeys > 0) { m_treasureKeys--; return true; } return false; }
    int  getTreasureKeys() const { return m_treasureKeys; }
    void pickupGoldenKey()       { m_hasGoldenKey = true; }
    bool hasGoldenKey() const    { return m_hasGoldenKey; }
    void addTreasure(int value)  { m_treasureScore += value; }
    int  getTreasureScore() const { return m_treasureScore; }
    void resetForNewLevel()      { m_hasGoldenKey = false; }
    void resetAll()              { m_treasureKeys = 0; m_hasGoldenKey = false; m_treasureScore = 0; }

private:
    int  m_treasureKeys  = 0;
    bool m_hasGoldenKey  = false;
    int  m_treasureScore = 0;
};
