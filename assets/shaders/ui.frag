#version 330 core

in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;

out vec4 FragColor;

void main()
{
    vec4 color = uColor;
    if (uUseTexture)
        color *= texture(uTexture, vTexCoord);
    FragColor = color;
}
