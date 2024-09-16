#version 440 core
layout (location = 0) in int aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 tilePos;
out int faceIndex;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{   
    vec3 position;
    position.z = aPos & 0x3F;
    position.y = (aPos >> 6) & 0x3F;
    position.x = (aPos >> 12) & 0x3F;
    gl_Position = projection * view * model * vec4(position, 1.0);

    FragPos = vec3(model * vec4(position, 1.0));
    faceIndex = aPos >> 18;
    tilePos = fract(aTexCoord);
    TexCoord = floor(aTexCoord);
}