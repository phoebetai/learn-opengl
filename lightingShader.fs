#version 460 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform Material material;

void main() {
	// Ambient light
	vec3 ambient = material.ambient * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);

	// Diffuse light
	float diff = max(dot(norm, lightDir), 0.0); // Clamp to maximum of 90 degrees between two vectors
	vec3 diffuse = diff * material.diffuse * lightColor;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // Reflected light vector

	// Specular light
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // The smaller the angle between view and reflection, the stronger the specular light
	vec3 specular = spec * material.specular * lightColor;

	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0);
}