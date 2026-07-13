#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 vColor;

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_LIGHTS 32
uniform PointLight pointLights[MAX_LIGHTS];
uniform int numLights;

uniform vec3 viewPos;
uniform sampler2D diffuseTex;
uniform bool useTexture;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    vec3 baseColor = useTexture ? vec3(texture(diffuseTex, TexCoords)) : vec3(vColor);
    
    for(int i = 0; i < numLights; i++) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        
        // ambient
        vec3 ambient = 0.15 * baseColor * pointLights[i].color;
        
        // diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * baseColor * pointLights[i].color;
        
        // attenuation
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));    
        
        result += (ambient + diffuse) * attenuation;
    }
    
    // Fallback if no lights
    if (numLights == 0) {
        result = baseColor * 0.2;
    }
    
    FragColor = vec4(result, vColor.a);
}
