#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec3 vColor;
out vec3 vLocalPos;
out vec3 vWorldPos;

void main() {
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    gl_Position = u_Proj * u_View * worldPos;
    vColor = aColor;
    vLocalPos = aPos;
    vWorldPos = worldPos.xyz;
}
