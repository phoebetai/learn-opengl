#version 460 core

struct Light {
	vec3 position;
	vec3 direction; // Directional light (light to fragment)
	float cutoff;      // Cosine of cutoff angle for spotlight
	float outerCutoff; // Cosine of outer cone angle for spotlight

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
	// Ambient light
	vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb;

	// Diffuse light
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos); // For positional light
	float diff = max(dot(norm, lightDir), 0.0); // Clamp to maximum of 90 degrees between two vectors
	vec3 diffuse = light.diffuse * (diff * texture(material.diffuse, texCoords).rgb);

	// Specular light
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // Reflected light vector
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // The smaller the angle between view and reflection, the stronger the specular light
	vec3 specular = light.specular * (spec * texture(material.specular, texCoords).rgb);

	// Calculate spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction)); // Angle between spotlight dir and fragment-to-light vec
	float epsilon = light.cutoff - light.outerCutoff; // Difference between inner and outer angles
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0); // 1.0 inside inner cone, 0.0 outside outer cone, and interpoalted in between
	diffuse  *= intensity;
	specular *= intensity;

	// Calculate attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	// Final color
	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0);
}