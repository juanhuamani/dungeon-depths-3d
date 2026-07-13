#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;

uniform mat4 uViewProjection;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 vColor;
out vec4 FragPosLightSpace;

void main()
{
    gl_Position = uViewProjection * vec4(aPos, 1.0);
    FragPos = aPos; // No model matrix applied here for tiles
    Normal = aNormal;
    TexCoords = aTexCoords;
    vColor = aColor;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}
