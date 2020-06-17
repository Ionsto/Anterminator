#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 Offset;
uniform vec2 Scale;

void main()
{
    //gl_Position = vec4((aPos + Offset) * Scale,1.0, 1.0);
    gl_Position = vec4((aPos * Scale) + Offset,1.0, 1.0);
//    gl_Position = vec4((aPos * Scale) + Offset,0, 1.0);
    TexCoord = vec2(aTexCoord.y,aTexCoord.x);
}