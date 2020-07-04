#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 CameraPosition;
uniform vec2 Scale;
uniform float WorldSize;
uniform mat4 MVP;

void main()
{
    gl_Position = vec4((aPos - CameraPosition) * Scale,0.0, 1);
    gl_Position = MVP * vec4(aPos * WorldSize,50.0, 1);
    TexCoord = vec2(aTexCoord.x,aTexCoord.y);
}