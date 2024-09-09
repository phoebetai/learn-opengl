#version 460 core

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	sampler2D diffuse; // This is an opaque data type (i.e. a handle)
	vec3 specular;
	float shininess;
};

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main() {
	// Ambient light
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);

	// Diffuse light
	float diff = max(dot(norm, lightDir), 0.0); // Clamp to maximum of 90 degrees between two vectors
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, texCoords)));

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // Reflected light vector

	// Specular light
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // The smaller the angle between view and reflection, the stronger the specular light
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0);
}