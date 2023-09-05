#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

//out vec3 objectColor;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 mvp;
uniform mat4 model;
uniform vec3 viewPos;
uniform vec3 lightPos;

void main(){
	gl_Position = mvp * vec4(aPos, 1.0f);
	TexCoords = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));

	Normal = normalize(vec3(model * vec4(aNormal, 0.0)));
}