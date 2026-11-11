#version 330 core

struct Material {
    sampler2D diff;
    sampler2D spec;
    sampler2D shininess;
};

struct Light {
    vec3 pos;
    vec3 amb;
    vec3 color;
};

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.pos - fragPos);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 specular = light.color * vec3(texture(material.spec, texCoord) * pow(max(dot(viewDir, reflectDir), 0.0f), 128.0f * float(texture(material.shininess, texCoord))));

    vec3 diffuse = max(dot(norm, lightDir), 0.0) * light.color * vec3(texture(material.diff, texCoord));

    FragColor = vec4(light.amb * vec3(texture(material.diff, texCoord)) + diffuse + specular, 1.0f);
}