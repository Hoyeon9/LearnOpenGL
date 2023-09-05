#version 330 core
//convenient form to store values about the object
struct Material{
	//vec3 ambient;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
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

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 lightColor;
uniform vec3 viewPos;



void main(){
	//ambient----
	//vec3 ambient = light.ambientStrength * lightColor * texture(material.texture_diffuse1, texCoord).rgb;
	vec3 ambient = light.ambientStrength * lightColor * vec3(1,1,1);

	//diffuse------
	vec3 nVec = normalize(Normal);
	//vec3 lVec = normalize(light.position - FragPos);
	vec3 lVec = normalize(light.direction); //directional
	float diffVal = max(dot(nVec, lVec), 0.0); // max: to make >= 0
	//vec3 diffuse = light.diffuseStrength * lightColor * diffVal * texture(material.texture_diffuse1, texCoord).rgb;
	vec3 diffuse = light.diffuseStrength * lightColor * diffVal *  vec3(1,1,1);

	//specular
	vec3 vVec = normalize(viewPos - FragPos);
	vec3 rVec = reflect(-lVec, nVec);
	float specVal = pow(max(dot(vVec, rVec), 0.0), material.shininess);
	//vec3 specular = light.specularStrength * lightColor * specVal * texture(material.texture_specular1, texCoord).rgb;
	vec3 specular = light.specularStrength * lightColor * specVal *  vec3(1,1,1);

	//attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance*distance));
	//ambient *= attenuation;
	//diffuse *= attenuation;
	//specular *= attenuation;
	
	//spolighting
	//float cosTheta = dot(lVec, normalize(-light.direction));
	//float epsilon = light.cutoff - light.outerCutoff;
	//float intensity = clamp((cosTheta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
	//diffuse *= intensity;
	//specular *= intensity;

	vec3 result = (diffuse  + ambient + specular);
	//if(cosTheta<=light.outerCutoff){
	//	result = ambient;
	//} 

    FragColor = vec4(result,1);
}