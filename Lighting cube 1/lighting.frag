#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
	// ambient light
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse light
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0); // 0 for directions away from light source, also want to avoid negative light
	vec3 diffuse = diff*lightColor;

	// specular light
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;


	vec3 result = (ambient + diffuse + specular) * objectColor;

	color = vec4(result, 1.0f);
}