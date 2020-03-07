#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 MatAmbi;
in vec3 MatDiff;
in vec3 MatSpec;
in float Shini;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform int diffuseN;
uniform int specularN;
uniform int nrComponents_d;
uniform int nrComponents_sp;
uniform bool spot;
uniform bool point;
uniform bool dir;

vec3 result;

// function prototypes
vec3 CalcDirectionLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
	if(diffuseN!=0){if(nrComponents_d==4){if(vec4(texture(material.texture_diffuse1,TexCoords)).w<0.1) discard;}}
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	//vec4 col =vec4(texture(material.texture_diffuse1, TexCoords));
	//float w=col.w;
	
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
	
	 result = CalcDirectionLight(dirLight, norm, viewDir);
	

    // phase 2: point lights
	if(point)
	{
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);  
	}
    // phase 3: spot light
	if(spot) result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
     

    FragColor = vec4(result, 1.0);
	//else FragColor = vec4(result, w);
	
}

// calculates the color when using a directional light.
vec3 CalcDirectionLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir=normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), Shini);
    // combine results
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff ;
	vec3 specular = light.specular * spec;
	
	if(diffuseN!=0)
	{
		 ambient *=  vec3(texture(material.texture_diffuse1, TexCoords));
		 diffuse *=  vec3(texture(material.texture_diffuse1, TexCoords));
	}
	else{
		ambient*=MatAmbi;
		diffuse*=MatDiff;
	}
	if(specularN!=0)
	specular *=  vec3(texture(material.texture_specular1, TexCoords));
	else specular*= MatSpec;

	return (ambient + diffuse + specular);
   
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir=normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), Shini);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
   
   vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff ;
	vec3 specular = light.specular * spec;
	
	if(diffuseN!=0)
	{
		 ambient *=  vec3(texture(material.texture_diffuse1, TexCoords));
		 diffuse *=  vec3(texture(material.texture_diffuse1, TexCoords));
	}
	else{
		ambient*=MatAmbi;
		diffuse*=MatDiff;
	}
	if(specularN!=0)
	specular *=  vec3(texture(material.texture_specular1, TexCoords));
	else specular*= MatSpec;

	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	return (ambient + diffuse + specular);
	
 
   
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shini);
	vec3 halfwayDir=normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), Shini);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff ;
	vec3 specular = light.specular * spec;
	
	if(diffuseN!=0)
	{
		 ambient *=  vec3(texture(material.texture_diffuse1, TexCoords));
		 diffuse *=  vec3(texture(material.texture_diffuse1, TexCoords));
	}
	else{
		ambient*=MatAmbi;
		diffuse*=MatDiff;
	}
	if(specularN!=0)
	specular *=  vec3(texture(material.texture_specular1, TexCoords));
	else specular*= MatSpec;

	ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
	

    
}