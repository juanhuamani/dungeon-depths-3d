#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform PointLight light;
uniform bool useTexture;

void main()
{
    vec3 ambient = light.ambient;
    if (useTexture) {
        ambient *= vec3(texture(material.diffuse, TexCoords));
    }
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseObj = useTexture ? vec3(texture(material.diffuse, TexCoords)) : vec3(0.8);
    vec3 diffuse = light.diffuse * diff * diffuseObj;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularObj = useTexture ? vec3(texture(material.specular, TexCoords)) : vec3(0.5);
    vec3 specular = light.specular * spec * specularObj;
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
