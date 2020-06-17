#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D DestTexture;

void main()
{
    FragColor = texture(DestTexture, TexCoord);
}
