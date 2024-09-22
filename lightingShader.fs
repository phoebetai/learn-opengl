#version 460 core

#define NUM_POINT_LIGHTS 4

struct Material {
	sampler2D diffuse; // This is an opaque data type (i.e. a handle)
	sampler2D specular;
	float shininess;
};

// An infinitely far light source
struct DirLight {
	vec3 direction; // All rays are parallel

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// An omnidirectional light with attenuation
struct PointLight {
	vec3 position;

	// Attenuation function coeffs
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// A directional cone light with attenuation
struct Spotlight {
	vec3 position;
	vec3 direction;
	float cutoff;      // Cosine of inner cone angle
	float outerCutoff; // Cosine of outer cone angle

	// Attenuation function coeffs
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform Material material;
uniform vec3 viewPos;

// Lights
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform Spotlight spotlight;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	// Fragment properties
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos); // Fragment to camera

	// Calculate directional lighting
	vec3 result = calcDirLight(dirLight, norm, viewDir);

	// Calculate point lights
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		result += calcPointLight(pointLights[i], norm, fragPos, viewDir);
	}

	// Calculate spotlight
	result += calcSpotlight(spotlight, norm, fragPos, viewDir);

	// Final color
	fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	// Ambient
	vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb; // Flat percentage of diffuse color

	// Diffuse
	vec3 lightDir = normalize(-light.direction);  // Fragment to light
	float diff = max(dot(normal, lightDir), 0.0); // How much the surface is facing away from the light
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoords).rgb;

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal); // Reflected light vector
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // The smaller the angle between view and reflected light vec, the sharper the hightlight
	vec3 specular = light.specular * spec * texture(material.specular, texCoords).rgb;
	return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// Ambient
	vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb;

	// Diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoords).rgb;

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, texCoords).rgb;

	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

vec3 calcSpotlight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// Ambient
	vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb;

	// Diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoords).rgb;

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, texCoords).rgb;

	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Intensity
	float theta = dot(lightDir, normalize(-light.direction)); // Angle between spotlight dir and fragment-to-light vec
	float epsilon = light.cutoff - light.outerCutoff; // Difference between inner and outer angles
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0); // 1.0 inside inner cone, 0.0 outside outer cone, and interpolated in between

	ambient  *= attenuation * intensity;
	diffuse  *= attenuation * intensity;
	specular *= attenuation * intensity;
	return ambient + diffuse + specular;
}