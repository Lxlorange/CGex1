#version 330 core
in vec3 vColor;
in vec3 vLocalPos;

uniform float u_Time;

out vec4 FragColor;

float linePulse(float t) {
    return smoothstep(0.92, 1.0, abs(sin(t)));
}

void main() {
    vec3 base = vColor;
    float gx = linePulse(vLocalPos.x * 11.0 + u_Time * 0.6);
    float gy = linePulse(vLocalPos.y * 11.0 - u_Time * 0.6);
    float pulse = 0.03 * sin(u_Time * 2.0 + (vLocalPos.x + vLocalPos.y) * 8.0);

    vec3 geek = base + vec3(0.00, 0.02, 0.05);
    geek += vec3((gx + gy) * 0.07);
    geek += vec3(pulse);

    FragColor = vec4(clamp(geek, 0.0, 1.0), 1.0);
}
