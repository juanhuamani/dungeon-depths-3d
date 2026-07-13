#version 330 core

in vec2 vTexCoord;
in vec4 vColor;
in float vTexIndex;

uniform sampler2D uTextures[8];

out vec4 FragColor;

void main()
{
    vec4 color = vColor;
    int index = int(vTexIndex + 0.5); // +0.5 for rounding safety
    
    if (index >= 0 && index < 8) {
        // Dynamic indexing of sampler array (supported on modern GLSL, standard 330+)
        // Alternatively, a switch statement for older cards:
        switch(index) {
            case 0: color *= texture(uTextures[0], vTexCoord); break;
            case 1: color *= texture(uTextures[1], vTexCoord); break;
            case 2: color *= texture(uTextures[2], vTexCoord); break;
            case 3: color *= texture(uTextures[3], vTexCoord); break;
            case 4: color *= texture(uTextures[4], vTexCoord); break;
            case 5: color *= texture(uTextures[5], vTexCoord); break;
            case 6: color *= texture(uTextures[6], vTexCoord); break;
            case 7: color *= texture(uTextures[7], vTexCoord); break;
        }
    }
    
    FragColor = color;
}
