#include "ui/InventoryUI.h"
#include "player/Player.h"
#include "player/Inventory.h"
#include "player/Item.h"

#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cctype>

bool InventoryUI::init()
{
    if (!m_shader.loadFromFiles("assets/shaders/ui.vert", "assets/shaders/ui.frag"))
    {
        std::cerr << "InventoryUI: Error cargando shaders\n";
        return false;
    }
    setupVAO();

    const char* abc = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 26; ++i)
    {
        std::string path = "assets/textures/Abcedario/";
        path += abc[i];
        path += ".PNG";
        m_abcedario[i] = loadTexture(path);
        if (!m_abcedario[i])
        {
            std::cerr << "InventoryUI: Falta letra " << abc[i] << "\n";
            return false;
        }
    }

    m_slotBlue = loadTexture("assets/textures/Item Slots/itemSlot_blue.png");
    m_slotBlack = loadTexture("assets/textures/Item Slots/itemSlot_black.png");
    m_slotAdvBlue = loadTexture("assets/textures/Item Slots/itemSlotAdvanced_blue.png");
    m_slotAdvBlack = loadTexture("assets/textures/Item Slots/itemSlotAdvanced_black.png");
    m_modalBg = loadTexture("assets/textures/Modals/modalSimple_v1.png");
    m_potionVida = loadTexture("assets/textures/Icons/pociones/pocion-vida.PNG");
    m_potionEnergia = loadTexture("assets/textures/Icons/pociones/pocion-energia.PNG");
    m_keyIcon = loadTexture("assets/textures/Icons/64px/key_icon_64px.png");
    m_coinIcon = loadTexture("assets/textures/Icons/64px/coin_icon_64px.png");
    m_chestIcon = loadTexture("assets/textures/Icons/64px/chestRuby_icon_64px.png");
    m_progressBarGreen = loadTexture("assets/textures/ProgressBars/progressBar_green.png");
    m_progressBarRed = loadTexture("assets/textures/ProgressBars/progressBar_red.png");
    m_effectYellow = loadTexture("assets/textures/Effects/effect_yellow.png");
    m_heartFull = loadTexture("assets/textures/Icons/heart/corazon-completo.PNG");
    m_heartHalf = loadTexture("assets/textures/Icons/heart/medio-corazon.PNG");
    m_heartEmpty = loadTexture("assets/textures/Icons/heart/corazon vacio.PNG");

    if (!m_slotBlue || !m_slotAdvBlue || !m_modalBg ||
        !m_potionVida || !m_potionEnergia || !m_keyIcon || !m_coinIcon ||
        !m_progressBarGreen || !m_progressBarRed || !m_effectYellow ||
        !m_heartFull || !m_heartEmpty)
    {
        std::cerr << "InventoryUI: Error cargando texturas\n";
        return false;
    }

    m_items[0] = {ItemType::HEALTH_POTION, "pocion de vida",   "restaura 1 hp",         "+1 hp",   m_potionVida};
    m_items[1] = {ItemType::ENERGY_POTION, "pocion energia",   "restaura energia",       "+energia", m_potionEnergia};
    m_items[2] = {ItemType::TREASURE_CHEST,"llave dorada",      "abre la salida del nivel", "llave", m_keyIcon};
    m_items[3] = {ItemType::TREASURE_CHEST,"tesoro",            "puntos de tesoro",       "puntos",  m_coinIcon};
    m_itemCount = 4;

    std::cout << "InventoryUI: OK\n";
    return true;
}

void InventoryUI::cleanup()
{
    for (auto& t : m_abcedario) { if (t) { glDeleteTextures(1, &t); t = 0; } }
    auto del = [](GLuint& t) { if (t) { glDeleteTextures(1, &t); t = 0; } };
    del(m_slotBlue); del(m_slotBlack); del(m_slotAdvBlue); del(m_slotAdvBlack);
    del(m_modalBg); del(m_potionVida); del(m_potionEnergia);
    del(m_keyIcon); del(m_coinIcon); del(m_chestIcon);
    del(m_progressBarGreen); del(m_progressBarRed); del(m_effectYellow);
    del(m_heartFull); del(m_heartHalf); del(m_heartEmpty);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    m_shader.destroy();
}

void InventoryUI::setupVAO()
{
    struct V { float x, y, u, v; };
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V) * 6, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

GLuint InventoryUI::loadTexture(const std::string& path)
{
    int w, h, ch;
    unsigned char* d = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!d) { std::cerr << "  X " << path << ": " << stbi_failure_reason() << "\n"; return 0; }
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(d);
    return t;
}

void InventoryUI::drawQuad(float x, float y, float w, float h, const glm::vec4& c)
{
    float x2 = x + w, y2 = y + h;
    struct { float x, y, u, v; } v[6] = {
        {x, y, 0, 0}, {x2, y, 0, 0}, {x2, y2, 0, 0},
        {x, y, 0, 0}, {x2, y2, 0, 0}, {x, y2, 0, 0}
    };
    m_shader.setInt("uUseTexture", 0);
    m_shader.setVec4("uColor", c);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v), v);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void InventoryUI::drawTex(float x, float y, float w, float h, GLuint tex, const glm::vec4& tint)
{
    if (!tex) { drawQuad(x, y, w, h, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)); return; }
    float x2 = x + w, y2 = y + h;
    struct { float x, y, u, v; } v[6] = {
        {x, y, 0, 0}, {x2, y, 1, 0}, {x2, y2, 1, 1},
        {x, y, 0, 0}, {x2, y2, 1, 1}, {x, y2, 0, 1}
    };
    m_shader.setInt("uUseTexture", 1);
    m_shader.setVec4("uColor", tint);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    m_shader.setInt("uTexture", 0);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v), v);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void InventoryUI::drawStr(const std::string& text, float x, float y, float s, const glm::vec4& tint)
{
    float gap = 0.0f;
    for (char c : text)
    {
        if (c == ' ') { x += s * 0.4f + gap; continue; }
        int idx = -1;
        if (c >= 'a' && c <= 'z') idx = c - 'a';
        else if (c >= 'A' && c <= 'Z') idx = c - 'A';
        else continue;
        if (idx >= 0 && idx < 26 && m_abcedario[idx])
        {
            drawTex(x, y, s, s, m_abcedario[idx], tint);
        }
        x += s * 0.4f + gap;
    }
}

void InventoryUI::render(int w, int h, const Player& p, const Item* chest,
                         bool showInv, int& sel, float cx, float cy, bool clicked)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_shader.use();
    m_shader.setMat4("projection",
        glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f));

    if (showInv) renderModal(w, h, p, sel, cx, cy, clicked);
    else renderHUD(w, h, p, chest);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void InventoryUI::renderHUD(int w, int h, const Player& p, const Item* chest)
{
    const Inventory& inv = p.getInventory();
    int potions = inv.getHealthPotions();
    int hp = p.getHealth(), maxHp = p.getMaxHealth();

    float slotSize = 56.0f, gap = 6.0f;
    float totalW = 4.0f * slotSize + 3.0f * gap;
    float sx = (w - totalW) * 0.5f, sy = h - slotSize - 16.0f;

    for (int i = 0; i < 4; ++i)
    {
        float x = sx + i * (slotSize + gap);
        drawTex(x, sy, slotSize, slotSize, m_slotBlue);
        if (i < potions)
        {
            float is = 32.0f, o = (slotSize - is) * 0.5f;
            drawTex(x + o, sy + o, is, is, m_potionVida);
        }
    }

    float hs = 24.0f, hg = 4.0f, hx = 14.0f, hy = 14.0f;
    for (int i = 0; i < maxHp; ++i)
    {
        float x = hx + i * (hs + hg);
        GLuint tex = (i < hp) ? m_heartFull : m_heartEmpty;
        drawTex(x, hy, hs, hs, tex);
    }

    if (chest && chest->isActive())
    {
        float pw = 140, ph = 40, px = (w - pw) * 0.5f, py = h * 0.40f;
        drawTex(px, py, pw, ph, m_slotAdvBlack);
        drawTex(px + 8, py + (ph - 26) * 0.5f, 26, 26, m_chestIcon);
        drawStr("e", px + 40, py + (ph - 20) * 0.5f, 20, glm::vec4(0.95f, 0.85f, 0.2f, 1.0f));
    }
}

void InventoryUI::renderModal(int w, int h, const Player& p, int& sel,
                               float cx, float cy, bool clicked)
{
    const Inventory& inv = p.getInventory();

    float mw = 820, mh = 500;
    float mx = (w - mw) * 0.5f, my = (h - mh) * 0.5f;
    drawTex(mx, my, mw, mh, m_modalBg);

    int total = m_itemCount;
    float leftW = 360.0f;
    float entryH = 80.0f;
    float entryGap = 6.0f;
    float listX = mx + 36;
    float listY = my + 120;

    auto getItemState = [&](int i, bool& disc, int& qty) {
        disc = false; qty = 0;
        if (m_items[i].type == ItemType::HEALTH_POTION)
            { disc = inv.hasDiscoveredHealth(); qty = inv.getHealthPotions(); }
        else if (m_items[i].type == ItemType::ENERGY_POTION)
            { disc = inv.hasDiscoveredEnergy(); qty = inv.getEnergyPotions(); }
    };

    for (int i = 0; i < total; ++i)
    {
        bool discovered; int qty;
        getItemState(i, discovered, qty);

        float ey = listY + i * (entryH + entryGap);
        bool hover = (cx >= listX && cx < listX + leftW &&
                      cy >= ey && cy < ey + entryH);

        if (hover && clicked && discovered) sel = i;

        if (sel == i)
            drawTex(listX - 4, ey - 4, leftW + 8, entryH + 8,
                    m_effectYellow, glm::vec4(1.0f, 0.9f, 0.3f, 0.4f));

        GLuint slotT = discovered ? m_slotAdvBlue : m_slotAdvBlack;
        glm::vec4 tint = discovered ? glm::vec4(1.0f) : glm::vec4(0.3f, 0.3f, 0.3f, 0.6f);

        float slotS = entryH - 12;
        drawTex(listX + 6, ey + 6, slotS, slotS, slotT, tint);

        float iconS = slotS - 14;
        drawTex(listX + 6 + (slotS - iconS) * 0.5f, ey + 6 + (slotS - iconS) * 0.5f,
                iconS, iconS, m_items[i].iconTex, tint);

        float tx = listX + slotS + 20;
        float ty = ey + 10;
        drawStr(m_items[i].name, tx, ty, 28,
                discovered ? glm::vec4(0.9f, 0.85f, 0.75f, 1.0f)
                           : glm::vec4(0.4f, 0.4f, 0.4f, 0.6f));

        if (discovered && qty > 0)
        {
            std::string qtyStr = "x" + std::to_string(qty);
            float qs = 22;
            float qx = listX + leftW - (qtyStr.length() * (qs * 0.4f)) - 8;
            drawStr(qtyStr, qx, ey + entryH - qs - 8, qs,
                    glm::vec4(0.5f, 0.85f, 0.5f, 1.0f));
        }
    }

    float rightX = mx + leftW + 50;
    float rightW = mw - leftW - 80;
    float rightY = my + 120;
    float rightH = mh - 140;

    if (sel >= 0 && sel < total)
    {
        bool discovered; int qty;
        getItemState(sel, discovered, qty);

        if (discovered)
        {
            float iconS = 110;
            float iconX = rightX + (rightW - iconS) * 0.5f;
            float iconY = rightY + 10;
            drawTex(iconX - 10, iconY - 10, iconS + 20, iconS + 20, m_slotAdvBlue);
            drawTex(iconX, iconY, iconS, iconS, m_items[sel].iconTex);

            float ly = iconY + iconS + 40;
            drawStr(m_items[sel].name, rightX + 10, ly, 32,
                    glm::vec4(0.9f, 0.85f, 0.7f, 1.0f));

            float descY = ly + 38;
            drawStr(m_items[sel].desc, rightX + 10, descY, 24,
                    glm::vec4(0.65f, 0.6f, 0.5f, 1.0f));

            float barY = descY + 40;
            drawTex(rightX + 10, barY, 170, 24, m_progressBarRed);

            float statW = 110;
            drawQuad(rightX + 190, barY, statW, 24,
                     glm::vec4(0.15f, 0.15f, 0.2f, 0.7f));
            drawStr(m_items[sel].statLabel, rightX + 194, barY + 3, 20,
                    glm::vec4(0.9f, 0.3f, 0.3f, 1.0f));

            float qtyY = rightY + rightH - 48;
            drawTex(rightX + 10, qtyY, rightW - 20, 38, m_progressBarGreen,
                    glm::vec4(0.95f, 0.95f, 0.95f, 0.8f));

            std::string qtyStr = "cantidad: " + std::to_string(qty);
            drawStr(qtyStr, rightX + 18, qtyY + 8, 22,
                    glm::vec4(1.0f, 1.0f, 1.0f, 0.9f));
        }
        else
        {
            float qs = 90;
            float qx = rightX + (rightW - qs) * 0.5f;
            float qy = rightY + 20;
            drawTex(qx, qy, qs, qs, m_slotAdvBlack);
            drawTex(qx + 20, qy + 20, qs - 40, qs - 40,
                    m_items[sel].iconTex, glm::vec4(0.3f, 0.3f, 0.3f, 0.6f));

            drawStr("???", rightX + (rightW - 3 * 28 * 0.4f) * 0.5f,
                    qy + qs + 28, 28, glm::vec4(0.4f, 0.4f, 0.4f, 0.6f));
        }
    }
}
