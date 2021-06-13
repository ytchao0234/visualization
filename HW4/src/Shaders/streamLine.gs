#version 460 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    float width;
    float value;
} gs_in[];

out float value;

void main() { 
    value = gs_in[0].value;
    vec4 direction = gl_in[1].gl_Position - gl_in[0].gl_Position;
    vec3 expand_dir = normalize(cross(direction.xyz, vec3(0.0, 0.0, 1.0)));
    vec4 expand_val = vec4(expand_dir * gs_in[1].width * 0.01, 0.0);

    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(expand_dir * gs_in[1].width * 0.008, 0.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4(expand_dir * gs_in[1].width * 0.008, 0.0);
    EmitVertex();
    
    EndPrimitive();
}