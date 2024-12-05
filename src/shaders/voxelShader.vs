#version 440 core
layout (location = 0) in int bitPackedData;
layout (location = 1) in int bitPackedData2;

out vec3 playerPos;
out vec2 TexCoord;
out vec2 tilePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 playerPosition;

out struct VertexData{
    vec3 localPosition;
    vec3 globalPosition;
    vec2 objectSize;
    vec2 UV;
} vertexData;

flat out struct FlatVertexData{
    int textureID;
    int normalIndex;
    int faceID;
} flatVertexData;


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
    flatVertexData.normalIndex = (bitPackedData >> 18) & 0x07;
    flatVertexData.textureID = (bitPackedData >> 21) & 0xFF;
    flatVertexData.faceID = (bitPackedData >> 29) & 0x03;

    vec3 position;
    position.z = bitPackedData & 0x3F;
    position.y = (bitPackedData >> 6) & 0x3F;
    position.x = (bitPackedData >> 12) & 0x3F;

    vertexData.localPosition = position;
    vertexData.globalPosition = vec3(model * vec4(position, 1.0));

    gl_Position = projection * view * vec4(vertexData.globalPosition, 1.0);

    vertexData.UV = getUVCoordinate(flatVertexData.textureID, flatVertexData.faceID);
    vertexData.objectSize = vec2(bitPackedData2 & 0x3F, (bitPackedData2 >> 6) & 0x3F);

    tilePos = fract(vertexData.UV);
    TexCoord = floor(vertexData.UV);
    
    playerPos = playerPosition;
}