#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 fragPos[];
in vec3 normal[];
in vec2 texCoords[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform float time;

vec4 explode(vec4 position, vec3 normal){
	float magnitude = 2.0;
	vec3 direction = normal * ((-sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}
vec3 getNormal(){
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main(){
	vec3 calcNormal = getNormal();

	gl_Position = explode(gl_in[0].gl_Position, calcNormal);
	FragPos = fragPos[0];
	Normal = normal[0];
	TexCoords = texCoords[0];
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, calcNormal);
	FragPos = fragPos[1];
	Normal = normal[1];
	TexCoords = texCoords[1];
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, calcNormal);
	FragPos = fragPos[2];
	Normal = normal[2];
	TexCoords = texCoords[2];
	EmitVertex();

	EndPrimitive();
}