#version 330
in vec2 newTexture;
in vec3 fragNormal;

out vec4 outColor;
uniform sampler2D samplerTexture;

void main()
{
    vec3 ambientLightIntensity = vec3(0.3f, 0.3f, 0.3f);
    vec3 sunLightIntensity = vec3(1.0,1.0,1.0);
    vec3 sunLightDirection = normalize(vec3(-05.0f,02.0f, 03.0f));
    //vec3 sunLightDirection = normalize(vec3(100000.0f,100000.0f, 100000.0f));

    vec4 texel = texture(samplerTexture, newTexture);

    vec3 lightIntensity = ambientLightIntensity + sunLightIntensity * max(dot(fragNormal, sunLightDirection), 0.0f);

    outColor = vec4(texel.rgb * lightIntensity, texel.a);
}