#include "render/Texture2D.h"
#include <iostream>
#include "stb_image.h"

namespace render {

Texture2D::Texture2D()
    : m_RendererID(0), m_Width(0), m_Height(0), m_BPP(0), m_FilePath("") {
}

Texture2D::~Texture2D() {
    if (m_RendererID) {
        glDeleteTextures(1, &m_RendererID);
    }
}

bool Texture2D::load(const std::string& path) {
    m_FilePath = path;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* localBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    if (!localBuffer) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (localBuffer) {
        stbi_image_free(localBuffer);
    }

    return true;
}

void Texture2D::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace render
