#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 vColor;
in vec4 FragPosLightSpace;

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
uniform sampler2D shadowMap;
uniform bool useTexture;

uniform vec3 dirLightDir;
uniform vec3 dirLightColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
        
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-dirLightDir);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    vec3 baseColor = useTexture ? vec3(texture(diffuseTex, TexCoords)) : vec3(vColor);
    
    // Directional light (casts shadows)
    vec3 dirL = normalize(-dirLightDir);
    float dirDiff = max(dot(norm, dirL), 0.0);
    vec3 dirDiffuse = dirLightColor * dirDiff * baseColor;
    vec3 dirAmbient = 0.1 * dirLightColor * baseColor;
    
    float shadow = ShadowCalculation(FragPosLightSpace);       
    result += (dirAmbient + (1.0 - shadow) * dirDiffuse);
    
    // Point lights (no shadows)
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
