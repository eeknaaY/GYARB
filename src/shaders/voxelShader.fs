#version 440 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 tilePos;
in vec3 FragPos;
flat in int faceIndex;

// texture samplers
uniform sampler2D ourTexture;

vec3 faceNormals[6] = vec3[6] (
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, -1.0, 0.0),
	vec3(-1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.0, -1.0)
	);

void main()
{
	vec2 dx = dFdx(TexCoord);
	vec2 dy = dFdy(TexCoord);
	vec2 texcoord = tilePos + vec2(1.0/16, 1.0/16) * fract(TexCoord);

	float ambientStrength = 0.1;

	vec3 lightDir = normalize(vec3(9999, 4000, 9999) - FragPos);
	float diff = max(dot(faceNormals[faceIndex], lightDir), 0);

	vec4 fragColor = (diff + ambientStrength) * textureGrad(ourTexture, texcoord, dx, dy);

	float gamma = 1.7;
    FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}

