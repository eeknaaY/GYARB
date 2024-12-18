#version 440 core
out vec4 FragColor;

in struct VertexData{
    vec3 localPosition;
    vec3 globalPosition;
    vec2 objectSize;
    vec2 UV;
} vertexData;

flat in struct FlatVertexData{
    int textureID;
    int normalIndex;
    int faceID;
} flatVertexData;

in vec3 playerPos;

// Dont know why I need to send these from vs, and why they dont work inside vertex.
in vec2 TexCoord;
in vec2 tilePos;

// texture samplers
uniform sampler2D textureAtlas;
uniform sampler2DArray shadowMap;
uniform mat4 viewMatrix;
uniform samplerCube skybox;

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

float farPlane = 800;
vec4 cascadePlaneDistances = vec4(farPlane / 16.f, farPlane / 4.f, farPlane / 2.f, farPlane);
int cascadeCount = 4;

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
    vec4 fragPosViewSpace = viewMatrix * vec4(vertexData.globalPosition, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int shadowLayer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            shadowLayer = i;
            break;
        }
    }
    if (shadowLayer == -1)
    {
        shadowLayer = cascadeCount;
    }
        

    vec3 fragPosition = vertexData.globalPosition;

    if (flatVertexData.normalIndex <= 1){
        if (shadowLayer == 1){
            fragPosition -= vec3(0.1, 0, 0.1);
        }

        if (shadowLayer == 2){
            fragPosition -= vec3(0.2, 0, 0.2);
        }
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[shadowLayer] * vec4(fragPosition, 1.0);

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = faceNormals[flatVertexData.normalIndex];
    vec3 sunPos = vec3(playerPos.x + 100, 100, playerPos.z + 100);
	vec3 lightDir = normalize(sunPos - vertexData.globalPosition);

    float bias = 0;

    if (shadowLayer == 0){
        bias = max(0.003 * (1.0 - dot(normal, lightDir)), 0.004);
    }

    if (shadowLayer == 1){
        bias = 0.018;
    }

    if (shadowLayer == 2){
        bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.02);
    }

    if (shadowLayer == cascadeCount){
        bias *= 1 / (farPlane * 0.5f);
    }
    else{
        bias *= 1 / (cascadePlaneDistances[shadowLayer] * 0.5f);
    }


    // PCF
    // float shadow = 0.0;
    // for(int x = -1; x <= 1; ++x)
    // {
    //     for(int y = -1; y <= 1; ++y)
    //     {
    //         float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, shadowLayer)).x; 
    //         shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
    //     }    
    // }
    // shadow /= 9.0;

    float shadow = currentDepth - bias > texture(shadowMap, vec3(projCoords.xy, shadowLayer)).x ? 1.0 : 0.0;
    
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
    float fog_mindist = 500.0;

    vec4 fogColor = texture(skybox, normalize(vertexData.globalPosition - playerPos));
    float dist = length(vertexData.globalPosition.xyz - playerPos);
    float fogFactor = (fog_maxdist - dist) / (fog_maxdist - fog_mindist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 sunPos = playerPos + 1000;
	vec3 lightDir = normalize(sunPos - vertexData.globalPosition);
	float diff = max(dot(faceNormals[flatVertexData.normalIndex], lightDir), 0);

	vec4 textureColor = textureGrad(textureAtlas, texcoord, dx, dy);
	float shadow = ShadowCalculation();
	vec4 fragColor = (ambientStrength + (1.0 - shadow) * diff) * textureColor;

    float gamma = 1.7;
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));

    vec4 colorMix = mix(fogColor, fragColor, fogFactor);
    FragColor = colorMix;
    FragColor.a = textureColor.a;

    //Water alpha
    if (flatVertexData.textureID == 17){
        FragColor.a = 0.8;
    }
}

