#version 440 core
layout (location = 0) in int bitPackedData;
layout (location = 1) in int bitPackedData2;

out int textureID;
out vec2 TexCoord;
out vec2 tilePos;
out int faceIndex;
out vec3 FragPos;
out vec3 playerPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 playerPosition;

vec2 getUVCoordinate(int tID, int uvID){
    tID -= 1;
    const float textureBlockSize = 0.0625;
    float u = textureBlockSize * (tID % 16);
    float v = textureBlockSize * (tID - tID % 16)/16.0;

    int blockWidth = bitPackedData2 & 0x3F;
    int blockHeight = (bitPackedData2 >> 6) & 0x3F;

    if (uvID == 1){
        u += blockWidth;
    }

    if (uvID == 2){
        v += blockHeight;
    }

    if (uvID == 3){
        u += blockWidth;
        v += blockHeight;
    }

    return vec2(u, v);
}

void main()
{   
    faceIndex = (bitPackedData >> 18) & 0x07;
    textureID = (bitPackedData >> 21) & 0xFF;
    int uvID = (bitPackedData >> 29) & 0x03;

    vec3 position;
    position.z = bitPackedData & 0x3F;
    position.y = (bitPackedData >> 6) & 0x3F;
    position.x = (bitPackedData >> 12) & 0x3F;

    gl_Position = projection * view * model * vec4(position, 1.0);

    FragPos = vec3(model * vec4(position, 1.0));

    vec2 uvCoords = getUVCoordinate(textureID, uvID);

    tilePos = fract(uvCoords);
    TexCoord = floor(uvCoords);
    
    playerPos = playerPosition;
}