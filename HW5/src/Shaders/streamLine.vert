#version 460 compatibility
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aWidth;
layout (location = 2) in float aValue;

out VS_OUT {
    float width;
    float value;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float valueMax;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vs_out.width = aWidth;
    vs_out.value = aValue / valueMax;
}