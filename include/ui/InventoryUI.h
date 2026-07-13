#pragma once

#include "ui/UIBatch.h"
#include "engine/ItemType.h"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

class Player;
class Item;

class InventoryUI {
public:
    bool init();
    void render(int screenWidth, int screenHeight, const Player& player,
                const Item* nearbyChest, bool showInventory,
                int& selectedItem, float cursorX, float cursorY, bool mouseClicked);
    void cleanup();

private:
    UIBatch m_batch;

    GLuint m_abcedario[26]{};
    GLuint m_slotBlue = 0, m_slotBlack = 0;
    GLuint m_slotAdvBlue = 0, m_slotAdvBlack = 0;
    GLuint m_modalBg = 0;
    GLuint m_potionVida = 0, m_potionEnergia = 0;
    GLuint m_heartFull = 0, m_heartHalf = 0, m_heartEmpty = 0;
    GLuint m_keyIcon = 0, m_coinIcon = 0;
    GLuint m_chestIcon = 0;
    GLuint m_progressBarGreen = 0, m_progressBarRed = 0;
    GLuint m_effectYellow = 0;

    struct ItemDef {
        ItemType type;
        const char* name;
        const char* desc;
        const char* statLabel;
        GLuint iconTex;
    };
    ItemDef m_items[4];
    int m_itemCount = 0;

    GLuint loadTexture(const std::string& path);
    void drawQuad(float x, float y, float w, float h, const glm::vec4& color);
    void drawTex(float x, float y, float w, float h, GLuint tex, const glm::vec4& tint = glm::vec4(1.0f));
    void drawStr(const std::string& text, float x, float y, float letterSize, const glm::vec4& tint);

    void renderHUD(int w, int h, const Player& p, const Item* chest);
    void renderModal(int w, int h, const Player& p, int& sel,
                     float cx, float cy, bool clicked);
};
