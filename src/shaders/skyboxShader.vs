#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projectionSkybox;
uniform mat4 viewSkybox;

void main()
{
    TexCoords = aPos;
    gl_Position = projectionSkybox * viewSkybox * vec4(aPos, 1.0);
}  