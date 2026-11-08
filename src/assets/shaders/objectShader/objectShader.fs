#version 330 core

in vec3 normal;
in vec3 fragPos;

uniform vec3 lightPos;

uniform vec4 baseColor;
uniform vec4 lightColor;
uniform vec4 ambientColor;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diffuse = max(dot(norm, lightDir), 0.0);

    FragColor = baseColor * (ambientColor + diffuse * lightColor);
}