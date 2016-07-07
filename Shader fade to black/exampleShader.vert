#version 330

layout (location = 0) in vec3 position; // corresponds to glVertexAttribPointer(0,...)
layout (location = 1) in vec2 texCoord; // corresponds to glVertexAttribPointer(1,...)

out vec2 outTexCoord; // transfer texture coordinates to fragment shader
out vec3 outPosition; // transfer position values to fragment shader

void main() {
	gl_Position = vec4(position, 1.0f);
	outTexCoord = texCoord;
	outPosition = position;
}