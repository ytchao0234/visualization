#version 460 compatibility
in vec3 pos;
in vec3 texCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 resolution;
uniform vec3 voxelSize;
uniform float gap;
uniform float adjust;
uniform float threshold;
uniform sampler3D tex3D;
uniform sampler1D tex1D;

uniform vec3 eyePos;
uniform vec3 eyeDir;

uniform vec3 LightPosition0;
uniform vec3 LightDirection0;
uniform vec3 LightIntensity0;
uniform float LightCutoff0;

bool inside(vec3 coord)
{
    if(coord.x >= 0.0f && coord.x <= 1.0f &&
       coord.y >= 0.0f && coord.y <= 1.0f &&
       coord.z >= 0.0f && coord.z <= 1.0f)
    {
        return true;
    }
    else return false;
}

void main()
{
    vec3 current_pos = pos;
    vec3 current_texCoord = texCoord;
    vec3 direction = normalize(eyeDir / voxelSize);

    vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    float T = 0.0f;

    while(T < threshold && inside(current_texCoord))
    {
        vec4 tex3 = texture3D(tex3D, current_texCoord);
        vec4 tex1 = texture1D(tex1D, tex3.a);

        T = T + tex1.a * (1-T);
        color = color + (vec4(tex1.rgb * tex1.a, tex1.a) * adjust);

        current_pos += gap * direction;
        vec3 temp = vec3(current_pos.x + resolution.x/2.0, current_pos.y + resolution.y/2.0, current_pos.z + resolution.z/2.0);
        current_texCoord = temp / resolution;
    }

    FragColor = color;
}