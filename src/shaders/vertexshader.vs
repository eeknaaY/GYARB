#version 440 core
layout (location = 0) in int aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 tilePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{   
    vec3 position;
    position.z = aPos & 0x3F;
    position.y = (aPos >> 6) & 0x3F;
    position.x = (aPos >> 12);
    gl_Position = projection * view * model * vec4(position, 1.0);

    tilePos = fract(aTexCoord);
    TexCoord = floor(aTexCoord);
}