#version 460 compatibility
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 fragPos;
out vec3 normal;
out float toClip;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 clipping;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    toClip = clipping.x * aPos.x + clipping.y * aPos.y + clipping.z * aPos.z + clipping.w;

    fragPos = vec3(model * vec4(aPos, 1.0f));
    normal = aNormal;
}