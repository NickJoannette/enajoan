
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform float time;

void main()
{
    aTexCoord = aPos;

    gl_Position = projection * view * vec4(aPos, 1.0);
}  