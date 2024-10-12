#version 440 core
out vec4 FragColor;

flat in int textureID;
flat in int faceIndex;
in vec2 TexCoord;
in vec2 tilePos;
in vec3 playerPos;

in vec3 FragPos;
in mat4 lightPositionMatrix;


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

float ShadowCalculation()
{   
    vec3 fragPosition = FragPos;

    if (faceIndex == 0){
        fragPosition -= vec3(0.15, 0, 0.15);
    }

    if (faceIndex == 1){
        fragPosition -= vec3(0.15, 0, 0.15);
    }

    vec4 fragPosLightSpace = lightPositionMatrix * vec4(fragPosition, 1.0);

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = faceNormals[faceIndex];
    vec3 sunPos = vec3(playerPos.x + 100, 100, playerPos.z + 100);
	vec3 lightDir = normalize(sunPos - FragPos);
    float bias = 0.0017;
    //max(0.005 * (1.0 - dot(normal, lightDir)), 0.004);
    float texelSize = 1.0 / 2048;


    // PCF
    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).x; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    //float shadow = currentDepth - bias > texture(shadowMap, projCoords.xy).x ? 1.0 : 0.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}  


void main()
{
    vec4 cursorColor = vec4(0.65, 0.65, 0.65, 1.0);
    if (gl_FragCoord.x < 807 && gl_FragCoord.x > 793 && gl_FragCoord.y < 452 && gl_FragCoord.y > 448){
        FragColor = cursorColor;
        return;
    }

    if (gl_FragCoord.x < 802 && gl_FragCoord.x > 798 && gl_FragCoord.y < 457 && gl_FragCoord.y > 443){
        FragColor = cursorColor;
        return;
    }

	vec2 dx = dFdx(TexCoord);
	vec2 dy = dFdy(TexCoord);
	vec2 texcoord = tilePos + vec2(1.0/16, 1.0/16) * fract(TexCoord);

	float ambientStrength = 0.1;
	
    float fog_maxdist = 600.0;
    float fog_mindist = 400.0;
    vec4 fog_color = vec4(0.4, 0.4, 0.4, 1.0);
    float dist = length(FragPos.xyz - playerPos);
    float fog_factor = (fog_maxdist - dist) /
                  (fog_maxdist - fog_mindist);
    fog_factor = 1;//clamp(fog_factor, 0.0, 1.0);

    vec3 sunPos = vec3(playerPos.x + 100, 100, playerPos.z + 100);
	vec3 lightDir = normalize(sunPos - FragPos);
	float diff = max(dot(faceNormals[faceIndex], lightDir), 0);

	vec4 color = textureGrad(ourTexture, texcoord, dx, dy);
	float shadow = ShadowCalculation();
	vec4 fragColor = (ambientStrength + (1.0 - shadow) * diff) * color;
    fragColor = mix(fog_color, fragColor, fog_factor);

	float gamma = 1.7;
    FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
    FragColor.a = color.a;

    //Water alpha
    if (textureID == 17){
        FragColor.a = 0.4;
    }

}

