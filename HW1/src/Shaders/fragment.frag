#version 460 compatibility
out vec4 FragColor;
  
in vec4 Color;

void main()
{
    FragColor = Color;
}