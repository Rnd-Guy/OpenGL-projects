#version 330

layout (location = 0) in vec3 position; // corresponds to glVertexAttribPointer(0,...)
layout (location = 1) in vec3 myColor; // corresponds to glVertexAttribPointer(1,...)

out vec3 outColor; // transfer color to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);
	outColor = myColor;
}