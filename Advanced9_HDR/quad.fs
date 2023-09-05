#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main(){
	vec3 hdrColor = texture(hdrBuffer, texCoord).rgb;

	//hdrColor = hdrColor / (hdrColor + vec3(1.0));
	
	hdrColor = vec3(1.0) - exp(-hdrColor * exposure);

    FragColor = vec4(hdrColor, 1.0);
}



