#version 330 core

in vec3 vNormal;
in vec3 vFragPos;

uniform vec3 uColor;

out vec4 FragColor;

void main()
{
    // Luz direccional simple desde arriba-adelante
    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.5));
    vec3 normal   = normalize(vNormal);

    // Componente ambiental
    float ambient = 0.25;

    // Componente difusa
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Color final
    vec3 result = uColor * (ambient + diffuse * 0.75);
    FragColor = vec4(result, 1.0);
}
