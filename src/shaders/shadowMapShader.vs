#version 330 core
layout (location = 0) in int bitPackedData;

uniform mat4 model;

void main()
{
    int textureID = (bitPackedData >> 21) & 0xFF;
    // Dont render glass in shadow map
    if (textureID == 7){
        gl_Position = vec4(0, 0, 0, 0);
        return;
    }

    vec3 position;
    position.z = bitPackedData & 0x3F;
    position.y = (bitPackedData >> 6) & 0x3F;
    position.x = (bitPackedData >> 12) & 0x3F;
    gl_Position = model * vec4(position, 1.0);
} 