#version 440 core
layout (location = 0) in int bitPackedData;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 tilePos;
out int faceIndex;
out vec3 FragPos;
out vec3 playerPos;
out vec4 FragPosLightSpace;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform vec3 playerPosition;


void main()
{   
    vec3 position;
    position.z = bitPackedData & 0x3F;
    position.y = (bitPackedData >> 6) & 0x3F;
    position.x = (bitPackedData >> 12) & 0x3F;
    gl_Position = projection * view * model * vec4(position, 1.0);

    FragPos = vec3(model * vec4(position, 1.0));
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    tilePos = fract(aTexCoord);
    TexCoord = floor(aTexCoord);
    
    faceIndex = bitPackedData >> 18;
    
    playerPos = playerPosition;
}