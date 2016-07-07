#version 330

in vec3 outColor;

out vec4 color; // final output

uniform sampler2D picture;

void main() {
	color = vec4(outColor/255, 1.0f);
}