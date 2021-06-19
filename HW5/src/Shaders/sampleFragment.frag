#version 460 compatibility
// in vec4 vertexColor;
// out vec4 FragColor;
// uniform vec4 ourColor;
// void main()
// {
//     //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
//     //FragColor = vertexColor;
//     FragColor = ourColor;
// }
out vec4 FragColor;
  
// in vec4 ourColor;
in vec2 TexCoord;

// uniform sampler2D ourTexture;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    // FragColor = texture2D(ourTexture, TexCoord) * ourColor;
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.5);
}