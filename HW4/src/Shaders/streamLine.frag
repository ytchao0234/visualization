#version 460 compatibility
in float value;

out vec4 FragColor;

uniform sampler1D tex1D;

void main()
{
    FragColor = texture1D(tex1D, value);
    // FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}