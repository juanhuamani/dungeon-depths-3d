#pragma once
#include <glm/glm.hpp>
#include "engine/ShaderProgram.h"

namespace render {

struct PointLight {
    glm::vec3 position;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
    
    PointLight() 
        : position(0.0f), ambient(0.1f), diffuse(0.8f), specular(1.0f), 
          constant(1.0f), linear(0.09f), quadratic(0.032f) {}
          
    void applyToShader(const ShaderProgram& shader, const std::string& name = "light") const {
        shader.setVec3(name + ".position", position);
        shader.setVec3(name + ".ambient", ambient);
        shader.setVec3(name + ".diffuse", diffuse);
        shader.setVec3(name + ".specular", specular);
        shader.setFloat(name + ".constant", constant);
        shader.setFloat(name + ".linear", linear);
        shader.setFloat(name + ".quadratic", quadratic);
    }
};

} // namespace render
