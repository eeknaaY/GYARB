#version 330 core
layout (location = 0) in int bitPackedData;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    int textureID = (bitPackedData >> 21) & 0xFF;
    if (textureID == 7){
        return;
    }

    vec3 position;
    position.z = bitPackedData & 0x3F;
    position.y = (bitPackedData >> 6) & 0x3F;
    position.x = (bitPackedData >> 12) & 0x3F;
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
} 