#version 330 core

in vec3 normal;
in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec4 baseColor;
uniform vec4 lightColor;
uniform vec4 ambientColor;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec4 specular = lightColor * pow(max(dot(viewDir, reflectDir), 0.0f), 64);

    vec4 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

    FragColor = baseColor * (ambientColor + diffuse + specular);
}