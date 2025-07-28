//Name: Defaultlit
//Type: Vertex

#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Proj;
} ubo;

void main() {
    gl_Position = ubo.u_Proj * ubo.u_View * ubo.u_Model * vec4(inPosition, 1.0);
}