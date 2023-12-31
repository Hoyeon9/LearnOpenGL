#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

//out vec3 objectColor;
out vec3 FragPos;
out vec3 Normal;
out vec2 texCoord;

uniform mat4 mvp;
uniform mat4 model;

void main(){
	gl_Position = mvp * vec4(aPos, 1.0f);
	//objectColor = aColor;
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;
	//Normal = mat3(transpose(inverse(model))); //costly operation
	texCoord = aTexCoord;
}