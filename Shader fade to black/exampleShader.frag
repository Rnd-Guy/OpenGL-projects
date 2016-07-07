#version 330

in vec2 outTexCoord;
in vec3 outPosition;

out vec4 color; // final output

uniform sampler2D picture;
uniform float count;

void main() {
	float multiplier = ((pow(outPosition.x+1, 1)*250 + 500) - count)/200;
	if (multiplier > 1) multiplier = 1;
	if (multiplier < 0) multiplier = 0;
	color = texture(picture, outTexCoord);
	color = vec4(color.rgb * multiplier, color.a);
	// fade transparent section to red, then to black
	if (color.a == 0) { 
		if (count < 750) color = vec4(1,0,0,count/750); // fade to red
		else color = vec4((1250-count)/500,0,0,1); // fade to black
	}
}
