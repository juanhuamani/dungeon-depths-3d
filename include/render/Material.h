#pragma once
#include "engine/ShaderProgram.h"
#include "render/Texture2D.h"
#include <memory>

namespace render {

struct Material {
    std::shared_ptr<Texture2D> diffuse;
    std::shared_ptr<Texture2D> specular;
    float shininess;
    
    Material() : shininess(32.0f) {}
    
    void applyToShader(const ShaderProgram& shader, const std::string& name = "material") const {
        if (diffuse) {
            diffuse->bind(0);
            shader.setInt(name + ".diffuse", 0);
            shader.setInt("useTexture", 1);
        } else {
            shader.setInt("useTexture", 0);
        }
        
        if (specular) {
            specular->bind(1);
            shader.setInt(name + ".specular", 1);
        } else {
            // Bind default or same as diffuse
            if(diffuse) diffuse->bind(1);
            shader.setInt(name + ".specular", 1);
        }
        
        shader.setFloat(name + ".shininess", shininess);
    }
};

} // namespace render
