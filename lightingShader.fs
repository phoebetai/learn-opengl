#version 460 core
out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	// Ambient light
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);

	// Diffuse light
	float diff = max(dot(norm, lightDir), 0.0); // Clamp to maximum of 90 degrees between two vectors
	vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // Reflected light vector

	// Specular light
	float specularStrength = 0.5;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // The smaller the angle between view and reflection, the stronger the specular light
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(result, 1.0);
}