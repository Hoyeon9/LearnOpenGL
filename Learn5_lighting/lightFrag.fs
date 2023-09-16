#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;

void main(){
	vec3 color = lightColor / (lightColor + vec3(1.0));
	FragColor = vec4(lightColor, 1.0);
}