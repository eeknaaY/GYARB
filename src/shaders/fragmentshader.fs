#version 440 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 tilePos;

// texture samplers
uniform sampler2D ourTexture;

void main()
{
	float u1 = TexCoord.x - float(trunc(TexCoord.x));
	float v1 = TexCoord.y - float(trunc(TexCoord.y));
	vec2 texCoord = tilePos + vec2(1.0/16, 1.0/16) * fract(TexCoord);
	// vec2 dx = dFdx(TexCoord);
	// vec2 dy = dFdy(TexCoord);
	// vec2 texcoord = vec2(0.0, 0.0) + vec2(1.0/16, 1.0/16) * fract(TexCoord);
	// vec4 color = textureGrad(ourTexture, texcoord, dx, dy);
	// FragColor = color;
	FragColor = texture(ourTexture, texCoord);
}