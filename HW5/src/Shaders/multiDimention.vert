#version 460 compatibility
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColor;

out float color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    color = aColor;
}