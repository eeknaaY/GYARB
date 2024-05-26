#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
// layout (location = 3) in mat4 instanceMatrix;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    // Like this -> gl_Position = projection * view * matrixArray[glInstanceID] * vec4(aPos, 1.0);
    
    // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}