#version 330 core
//convenient form to store values about the object
struct Material{
	//vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct Light{
	vec3 position;
	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D emission;



void main(){
	//ambient----
	vec3 ambient = light.ambientStrength * lightColor * texture(material.diffuse, texCoord).rgb;

	//diffuse------
	vec3 nVec = normalize(Normal);
	//vec3 lVec = normalize(lightPos - FragPos);
	vec3 lVec = normalize(lightPos - FragPos);
	float diffVal = max(dot(nVec, lVec), 0.0); // max: to make >= 0
	vec3 diffuse = light.diffuseStrength * lightColor * diffVal * texture(material.diffuse, texCoord).rgb;

	//specular
	vec3 vVec = normalize(viewPos - FragPos);
	vec3 rVec = reflect(-lVec, nVec);
	float specVal = pow(max(dot(vVec, rVec), 0.0), material.shininess);
	vec3 specular = light.specularStrength * lightColor * specVal * texture(material.specular, texCoord).rgb;

	vec3 result = (diffuse  + ambient + specular);
    FragColor = vec4(result,1);//+texture(emission, texCoord);
    //FragColor = texture(emission, texCoord);
}