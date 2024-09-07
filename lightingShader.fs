#version 460 core
out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
	fragColor = vec4(lightColor * objectColor, 1.0);
}