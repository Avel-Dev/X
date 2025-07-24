//Name: Defaultlit
//Type: Fragment

#version 450

layout(set = 1, binding = 0) uniform MeshColor
{
    vec4 u_Color;
} fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor.u_Color;
}
