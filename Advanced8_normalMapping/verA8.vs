#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

//out vec3 objectColor;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 TanLightPos;
out vec3 TanViewPos;
out vec3 TanFragPos;
out mat3 TBN;

uniform mat4 mvp;
uniform mat4 model;
uniform vec3 viewPos;
uniform vec3 lightPos;

void main(){
	gl_Position = mvp * vec4(aPos, 1.0f);
	TexCoords = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));

	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	vec3 B = cross(N, T);
	TBN = mat3(T, B, N);
	mat3 revTBN = transpose(mat3(T, B, N));
	TanLightPos = revTBN * lightPos;
	TanViewPos = revTBN * viewPos;
	TanFragPos = revTBN * vec3(model * vec4(aPos, 1.0));
}