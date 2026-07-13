#pragma once

#include "ui/UIBatch.h"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

namespace ui {

class MenuUI {
public:
    bool init();
    void renderMainMenu(int screenWidth, int screenHeight, int selection);
    void renderGameOver(int screenWidth, int screenHeight, bool victory);
    void cleanup();

private:
    UIBatch m_batch;
    GLuint m_abcedario[26]{};
    GLuint m_modalBg = 0;
    
    GLuint loadTexture(const std::string& path);
    void drawStr(const std::string& text, float x, float y, float size, const glm::vec4& tint);
};

} // namespace ui
