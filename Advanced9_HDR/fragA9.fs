#version 330 core
//convenient form to store values about the object
uniform sampler2D wood;

struct Light{
	vec3 position; //for point or spotlight
	vec3 direction; //directional light!
	float cutoff; //for spotlight
	float outerCutoff; //for spotlight
	float constant;
	float linear;
	float quadratic;
	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;
};

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

out vec4 FragColor;

uniform Light light;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // simple attenuation
    //float max_distance = 1.5;
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (distance * distance);
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}

void main(){
    vec3 result = vec3(0, 0, 0);
    for(int i=0; i<4; i++){
        	//ambient----
	    vec3 ambient = light.ambientStrength * lightColors[i] * texture(wood, TexCoords).rgb;
	    //vec3 ambient = light.ambientStrength * lightColor * vec3(1,1,1);

	    vec3 diffSpec = texture(wood, TexCoords).rgb * BlinnPhong(Normal, FragPos, lightPositions[i], lightColors[i]);

        result += (ambient + diffSpec);
    }

    FragColor = vec4(result,1);
}