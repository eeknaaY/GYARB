#version 440 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 tilePos;

// texture samplers
uniform sampler2D ourTexture;

void main()
{
	vec2 dx = dFdx(TexCoord);
	vec2 dy = dFdy(TexCoord);
	vec2 texcoord = tilePos + vec2(1.0/16, 1.0/16) * fract(TexCoord);
	vec4 color = textureGrad(ourTexture, texcoord, dx, dy);

	FragColor = color;
}