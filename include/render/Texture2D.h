#pragma once
#include <string>
#include <glad/gl.h>

namespace render {

class Texture2D {
public:
    Texture2D();
    ~Texture2D();

    // Loads a texture from file, returns true if successful.
    bool load(const std::string& path);
    void bind(unsigned int slot = 0) const;
    void unbind() const;

    unsigned int getID() const { return m_RendererID; }
    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }

private:
    unsigned int m_RendererID;
    int m_Width, m_Height, m_BPP;
    std::string m_FilePath;
};

} // namespace render
