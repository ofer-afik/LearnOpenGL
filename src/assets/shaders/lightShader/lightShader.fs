#version 330 core

struct Light {
    vec3 dir;
    vec3 amb;
    vec3 color;
};

uniform Light light;

out vec4 FragColor;

void main() {
    FragColor = vec4(light.color, 1.0f);
}