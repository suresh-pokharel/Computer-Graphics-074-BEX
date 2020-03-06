#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in vec3 aAmbient;
layout (location = 6) in vec3 aDiffuse;
layout (location = 7) in vec3 aSpecular;
layout (location = 8) in float aShininess;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 MatAmbi;
out vec3 MatDiff;
out vec3 MatSpec;
out float Shini;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;

	MatAmbi =aAmbient;
	MatDiff =aDiffuse;
	MatSpec = aSpecular;
	Shini = aShininess;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}