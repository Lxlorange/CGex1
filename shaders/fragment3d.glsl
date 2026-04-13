#version 330 core
in vec3 vColor;
in vec3 vLocalPos;
in vec3 vWorldPos;

uniform float u_Time;
uniform float u_IsWireframe;

out vec4 FragColor;

void main() {
    if (u_IsWireframe > 0.5) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 localN = normalize(vLocalPos);
    float rim = pow(1.0 - abs(localN.z), 2.2);
    float pulse = 0.06 * sin(u_Time * 1.7 + dot(vWorldPos, vec3(2.7, 1.9, 3.1)));

    vec3 base = pow(vColor, vec3(0.92));
    vec3 cyber = mix(base, vec3(0.08, 0.85, 1.0), rim * 0.25);
    cyber += vec3(0.02, 0.01, 0.04) + vec3(pulse);

    FragColor = vec4(clamp(cyber, 0.0, 1.0), 1.0);
}