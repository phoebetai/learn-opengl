#version 460 core

struct Light {
	vec3 position;
	vec3 direction; // Directional light (light to fragment)
	float cutoff;   // Cosine of cutoff angle for spotlight

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Material {
	sampler2D diffuse; // This is an opaque data type (i.e. a handle)
	sampler2D specular;
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
	vec3 lightDir = normalize(light.position - fragPos); // For positional light
	//vec3 lightDir = normalize(-light.direction); // Fragment to light source

	// Angle between spotlight direction and fragment-to-light vector
	float theta = dot(lightDir, normalize(-light.direction));
	if (theta > light.cutoff) {
		// Calculate attenuation
		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		// Ambient light
		vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
		ambient *= attenuation;

		// Diffuse light
		vec3 norm = normalize(normal);
		float diff = max(dot(norm, lightDir), 0.0); // Clamp to maximum of 90 degrees between two vectors
		vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, texCoords)));
		diffuse *= attenuation;

		// Specular light
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm); // Reflected light vector
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // The smaller the angle between view and reflection, the stronger the specular light
		vec3 specular = light.specular * (spec * vec3(texture(material.specular, texCoords)));
		specular *= attenuation;

		vec3 result = ambient + diffuse + specular;
		fragColor = vec4(result, 1.0);
	} else {
		// Use ambient lighting for anything not lit by spotlight
		fragColor = vec4(light.ambient * vec3(texture(material.diffuse, texCoords)), 1.0);
	}
}