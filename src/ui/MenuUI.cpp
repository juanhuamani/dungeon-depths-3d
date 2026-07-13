#include "ui/MenuUI.h"
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace ui {

bool MenuUI::init() {
    if (!m_batch.init()) {
        std::cerr << "MenuUI: Error init UIBatch\n";
        return false;
    }

    const char* abc = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 26; ++i) {
        std::string path = "assets/textures/Abcedario/";
        path += abc[i];
        path += ".PNG";
        m_abcedario[i] = loadTexture(path);
    }
    
    m_modalBg = loadTexture("assets/textures/Modals/modalCastle.png");
    
    return true;
}

void MenuUI::cleanup() {
    for (auto& t : m_abcedario) {
        if (t) { glDeleteTextures(1, &t); t = 0; }
    }
    if (m_modalBg) { glDeleteTextures(1, &m_modalBg); m_modalBg = 0; }
    m_batch.shutdown();
}

GLuint MenuUI::loadTexture(const std::string& path) {
    int w, h, ch;
    unsigned char* d = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!d) return 0;
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(d);
    return t;
}

void MenuUI::drawStr(const std::string& text, float x, float y, float s, const glm::vec4& tint) {
    float gap = 0.0f;
    for (char c : text) {
        if (c == ' ') { x += s * 0.4f + gap; continue; }
        int idx = -1;
        if (c >= 'a' && c <= 'z') idx = c - 'a';
        else if (c >= 'A' && c <= 'Z') idx = c - 'A';
        
        if (idx >= 0 && idx < 26 && m_abcedario[idx]) {
            m_batch.draw(x, y, s, s, m_abcedario[idx], tint);
        }
        x += s * 0.4f + gap;
    }
}

void MenuUI::renderMainMenu(int w, int h, int selection) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_batch.begin(glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f));
    
    // Dim background
    m_batch.draw(0, 0, w, h, 0, glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));
    
    float mw = 600, mh = 400;
    float mx = (w - mw) * 0.5f;
    float my = (h - mh) * 0.5f;
    if (m_modalBg) {
        m_batch.draw(mx, my, mw, mh, m_modalBg, glm::vec4(1.0f));
    } else {
        m_batch.draw(mx, my, mw, mh, 0, glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    }
    
    std::string title = "dungeon depths";
    float ts = 48.0f;
    float tx = mx + (mw - title.length() * (ts * 0.4f)) * 0.5f;
    drawStr(title, tx, my + 80, ts, glm::vec4(0.9f, 0.8f, 0.2f, 1.0f));
    
    std::string opt1 = "jugar";
    std::string opt2 = "salir";
    float ps = 32.0f;
    
    float px1 = mx + (mw - opt1.length() * (ps * 0.4f)) * 0.5f;
    float px2 = mx + (mw - opt2.length() * (ps * 0.4f)) * 0.5f;
    
    glm::vec4 c1 = (selection == 0) ? glm::vec4(1.0f, 1.0f, 0.2f, 1.0f) : glm::vec4(0.7f);
    glm::vec4 c2 = (selection == 1) ? glm::vec4(1.0f, 1.0f, 0.2f, 1.0f) : glm::vec4(0.7f);
    
    drawStr(opt1, px1, my + 200, ps, c1);
    drawStr(opt2, px2, my + 260, ps, c2);
    
    m_batch.end();
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void MenuUI::renderGameOver(int w, int h, bool victory) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_batch.begin(glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f));
    
    // Dim background
    glm::vec4 bgCol = victory ? glm::vec4(0.2f, 0.4f, 0.2f, 0.7f) : glm::vec4(0.4f, 0.1f, 0.1f, 0.7f);
    m_batch.draw(0, 0, w, h, 0, bgCol);
    
    float mw = 600, mh = 300;
    float mx = (w - mw) * 0.5f;
    float my = (h - mh) * 0.5f;
    if (m_modalBg) {
        m_batch.draw(mx, my, mw, mh, m_modalBg, glm::vec4(1.0f));
    } else {
        m_batch.draw(mx, my, mw, mh, 0, glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    }
    
    std::string title = victory ? "victoria" : "fin del juego";
    float ts = 48.0f;
    float tx = mx + (mw - title.length() * (ts * 0.4f)) * 0.5f;
    drawStr(title, tx, my + 80, ts, victory ? glm::vec4(0.3f, 0.9f, 0.3f, 1.0f) : glm::vec4(0.9f, 0.3f, 0.3f, 1.0f));
    
    std::string prompt = "presiona enter al menu";
    float ps = 24.0f;
    float px = mx + (mw - prompt.length() * (ps * 0.4f)) * 0.5f;
    drawStr(prompt, px, my + 200, ps, glm::vec4(1.0f));
    
    m_batch.end();
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

} // namespace ui
