#version 440 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 tilePos;
in vec3 FragPos;
in vec3 playerPos;
in vec4 FragPosLightSpace;
flat in int faceIndex;

// texture samplers
uniform sampler2D ourTexture;
uniform sampler2D shadowMap;

vec3 faceNormals[6] = vec3[6] (
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, -1.0, 0.0),
	vec3(-1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.0, -1.0)
);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(faceNormals[faceIndex]);
    vec3 sunPos = vec3(playerPos.x + 100, 100, playerPos.z + 100);
	vec3 lightDir = normalize(sunPos - FragPos);
    float bias = max(0.1 * (1.0 - dot(normal, lightDir)), 0.01);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}  

void main()
{
	vec2 dx = dFdx(TexCoord);
	vec2 dy = dFdy(TexCoord);
	vec2 texcoord = tilePos + vec2(1.0/16, 1.0/16) * fract(TexCoord);

	float ambientStrength = 0.1;
	
    float fog_maxdist = 360.0;
    float fog_mindist = 200.0;
    vec4 fog_color = vec4(0.4, 0.4, 0.4, 1.0);
    float dist = length(FragPos.xyz - playerPos);
    float fog_factor = (fog_maxdist - dist) /
                  (fog_maxdist - fog_mindist);
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    vec3 sunPos = vec3(playerPos.x + 100, 100, playerPos.z + 100);
	vec3 lightDir = normalize(sunPos - FragPos);
	float diff = max(dot(faceNormals[faceIndex], lightDir), 0);

	vec4 color = textureGrad(ourTexture, texcoord, dx, dy);
	float shadow = ShadowCalculation(FragPosLightSpace);
	vec4 fragColor = (ambientStrength + (1.0 - shadow) * diff) * color;
    fragColor = mix(fog_color, fragColor, fog_factor);

	float gamma = 1.7;
    FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}
