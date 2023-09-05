#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main(){
	//FragColor = texture(screenTexture, TexCoords);

	//inversion
	//FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

	//grayscale
	FragColor = texture(screenTexture, TexCoords);
	float avg = 0.2126 * FragColor.r + 0.7152*FragColor.g + 0.0722 * FragColor.b;
	FragColor = vec4(avg, avg, avg, 1.0);

	//kernel
	float offset = 1.0/ 300.0;
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
	);
	float kernelSharpen[9] = float[](
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	);
	float kernelBlur[9] = float[](
		1.0/16, 2.0/16, 1.0/16,
		2.0/16, 4.0/16, 2.0/16,
		1.0/16, 2.0/16, 1.0/16
	);
	float kernelEdge[9] = float[](
		1,1,1,
		1,-8,1,
		1,1,1
	);

	vec3 color = vec3(0.0);
	for(int i=0; i<9; i++){
		color += vec3(texture(screenTexture, TexCoords.st + offsets[i])) * kernelEdge[i];
	}
	FragColor = vec4(color, 1.0);
}