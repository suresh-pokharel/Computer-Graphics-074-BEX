#version 330 core

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float material_shininess;

//    sampler2D texture_diffuse1;
//    sampler2D texture_specular1;
//    float material_shininess;



/* Note: because we now use a material struct again you want to change your
mesh class to bind all the textures using material.texture_diffuseN instead of
texture_diffuseN. */

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 5

in vec3 fragPosition;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];

// Function prototypes
vec3 CalcPointLight(PointLight light, sampler2D mdiffuse, sampler2D mspecular, float mshininess, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    vec3 result=vec3(0.0);
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 norm = normalize(Normal);
    
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], texture_diffuse1, texture_specular1, material_shininess, norm, fragPosition, viewDir);
        
    color = vec4(result, 1.0f);
}


// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, sampler2D mdiffuse, sampler2D mspecular, float mshininess, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mshininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(mdiffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mdiffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(mspecular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}