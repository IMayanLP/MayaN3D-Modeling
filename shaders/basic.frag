#version 460 core

in vec3 normal;

out vec4 FragColor;

void main() {
    vec3 lightDirection = normalize(vec3(-1.0, 2.0, 1.0));
    float intensity = max(dot(normalize(normal), lightDirection), 0.0);
    vec3 objectColor = vec3(0.0, 0.8, 1.0);

    float ambient = 0.3;

    vec3 color = objectColor * min(ambient + intensity, 1.0);

    FragColor = vec4(color, 1.0);
}