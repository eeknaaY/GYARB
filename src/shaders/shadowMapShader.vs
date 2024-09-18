#version 330 core
layout (location = 0) in int aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    vec3 position;
    position.z = aPos & 0x3F;
    position.y = (aPos >> 6) & 0x3F;
    position.x = (aPos >> 12) & 0x3F;
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
} 