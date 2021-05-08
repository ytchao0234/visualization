#version 460 compatibility
in vec3 fragPos;
in vec3 texCoord;

out vec4 FragColor;

uniform sampler3D texture_3D;
uniform sampler1D texture_1D;

void main()
{
    vec4 tex_color = texture3D(texture_3D, texCoord);

    if(tex_color.b == 0)
        FragColor = vec4(1.0, 0.0, 0.0, 0.5);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 0.5);

    // FragColor = vec4(tex_color.rgb, 1.0);
}