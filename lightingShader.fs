#version 460 core
out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 result = ambient * objectColor;
	fragColor = vec4(result, 1.0);
}