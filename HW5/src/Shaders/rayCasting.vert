#version 460 compatibility
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;

out vec3 pos;
out vec3 texCoord;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    pos = aPos;
    texCoord = aTexCoord;
    fragPos = vec3(model * vec4(aPos, 1.0f));
}