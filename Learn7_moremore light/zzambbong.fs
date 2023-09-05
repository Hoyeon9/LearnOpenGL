#version 330 core
//convenient form to store values about the object
struct Material{
	//vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
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
struct DirLight{
	vec3 direction; //directional light!
	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;
};
struct PointLight{
	vec3 position; //for point or spotlight
	float constant;
	float linear;
	float quadratic;

	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;
};
struct SpotLight{
	vec3 position; //for point or spotlight
	vec3 direction;
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
in vec3 Normal;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform DirLight dirLight;
#define NUM_POINTS 4
uniform PointLight pointLights[NUM_POINTS];
uniform SpotLight spotLight;

uniform sampler2D emission;

vec3 calcDirLight(DirLight light, vec3 nVec, vec3 vVecr);
vec3 calcPointLight(PointLight light, vec3 nVec, vec3 FragPos, vec3 vVec);
vec3 calcSpotLight(SpotLight light, vec3 nVec, vec3 FragPos, vec3 vVec);
void main(){
	//ambient----
	vec3 nVec = normalize(Normal);
	vec3 vVec = normalize(viewPos - FragPos);

	//Directional
	vec3 result = calcDirLight(dirLight, nVec, vVec);
	//Point lights
	for(int i = 0; i<NUM_POINTS; i++){
		result += calcPointLight(pointLights[i], nVec, FragPos, vVec);
	}
	//spotlight
	result += calcSpotLight(spotLight, nVec, FragPos, vVec);

    FragColor = vec4(result,1);
}

vec3 calcDirLight(DirLight light, vec3 nVec, vec3 vVec){
	vec3 lVec = normalize(-light.direction);

	vec3 ambient = light.ambientStrength * lightColor * texture(material.diffuse, texCoord).rgb;
	float diffVal = max(dot(nVec, lVec), 0.0); // max: to make >= 0
	vec3 diffuse = light.diffuseStrength * lightColor * diffVal * texture(material.diffuse, texCoord).rgb;
	vec3 rVec = reflect(-lVec, nVec);
	float specVal = pow(max(dot(vVec, rVec), 0.0), material.shininess);
	vec3 specular = light.specularStrength * lightColor * specVal * texture(material.specular, texCoord).rgb;

	return (ambient + diffuse + specular);
}
vec3 calcPointLight(PointLight light, vec3 nVec, vec3 FragPos, vec3 vVec){
	vec3 lVec = normalize(light.position-FragPos);

	vec3 ambient = light.ambientStrength * lightColor * texture(material.diffuse, texCoord).rgb;
	float diffVal = max(dot(nVec, lVec), 0.0); // max: to make >= 0
	vec3 diffuse = light.diffuseStrength * lightColor * diffVal * texture(material.diffuse, texCoord).rgb;
	vec3 rVec = reflect(-lVec, nVec);
	float specVal = pow(max(dot(vVec, rVec), 0.0), material.shininess);
	vec3 specular = light.specularStrength * lightColor * specVal * texture(material.specular, texCoord).rgb;

	//attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance*distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
vec3 calcSpotLight(SpotLight light, vec3 nVec, vec3 FragPos, vec3 vVec){
	vec3 lVec = normalize(light.position-FragPos);

	vec3 ambient = light.ambientStrength * lightColor * texture(material.diffuse, texCoord).rgb;
	float diffVal = max(dot(nVec, lVec), 0.0); // max: to make >= 0
	vec3 diffuse = light.diffuseStrength * lightColor * diffVal * texture(material.diffuse, texCoord).rgb;
	vec3 rVec = reflect(-lVec, nVec);
	float specVal = pow(max(dot(vVec, rVec), 0.0), material.shininess);
	vec3 specular = light.specularStrength * lightColor * specVal * texture(material.specular, texCoord).rgb;

	//attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance*distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float cosTheta = dot(lVec, normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((cosTheta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
	diffuse *= intensity;
	specular *= intensity;

	vec3 result = (diffuse  + ambient + specular);

	if(cosTheta<=light.outerCutoff){
		result = ambient;
	} 

	return (ambient + diffuse + specular);
}