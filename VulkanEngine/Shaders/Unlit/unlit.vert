//Name: Unlit
//Type: Vertex

#version 450

layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Proj;
} ubo;

layout (set = 0,binding = 1) uniform Color {
   vec3 inColor;
} cor;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.u_Proj * ubo.u_View * ubo.u_Model * vec4(inPosition, 1.0);
    fragColor = cor.inColor;
    fragTexCoord = vec2(inTexCoord.x,inTexCoord.y);
}