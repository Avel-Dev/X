//Name: Unlit
//Type: Fragment

#version 450

layout(set = 2, binding = 0) uniform sampler2D Diffuse_texture;  // Input from vertex shader
layout(location = 0) in vec3 fragColor;  // Input from vertex shader
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;  // Output color

void main() {
	outColor = texture(Diffuse_texture, fragTexCoord);  // Set output color to the input color with full opacity)
}