#version 330 core

in vec3 ourColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform sampler2D texture2;


void main(){
	FragColor = mix(texture(ourTexture,texCoord), texture(texture2,texCoord), 0.2);
   //FragColor = texture(texture2, texCoord);
   //FragColor = vec4(ourColor,1);
}