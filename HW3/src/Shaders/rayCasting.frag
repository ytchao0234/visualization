#version 460 compatibility
in vec3 fragPos;
in vec3 texCoord;

out vec4 FragColor;

uniform sampler3D tex3D;
uniform sampler1D tex1D;

uniform vec3 eyePos;

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
    // FragColor = vec4(texCoord, 1.0f);
    // return;
    vec4 texture_color = texture3D(tex3D, texCoord);
    // FragColor = vec4(texture_color.rgb, 1.0);

    // vec3 tex3Coord = texCoord;
    // vec3 direction = normalize(fragPos - eyePos);
    // float gap = 0.5f;
    // vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    // float T = 0.0f;
    // float threshold = 0.5f;

    // while(T < threshold && inside(tex3Coord))
    // {
    //     vec4 tex3 = texture3D(texture_3D, tex3Coord);
    //     vec4 tex1 = texture1D(texture_1D, tex3.a);

    //     if(tex1.a > 1.0f)
    //     {
    //         FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //         return;
    //     }

    //     vec4 thisColor = vec4(1.0f, 0.0f, 0.0f, 0.1f);
    //     color = color + thisColor * (1-T);
    //     T = T + (1-T) * thisColor.a;
    //     tex3Coord = tex3Coord + gap * direction;
    // }

    vec4 tex1 = texture1D(tex1D, texture_color.a);

    // if(tex1.r > 0.0f)
    //     FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    // else
    FragColor = tex1;

    // FragColor = texture_color;
}