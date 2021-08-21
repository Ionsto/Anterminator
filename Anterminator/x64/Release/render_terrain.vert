#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 CameraPosition;
uniform vec2 Scale;


uniform float WorldSize;
uniform sampler2D DestTexture;
uniform mat4 MVP;

void main()
{
    TexCoord = vec2(aTexCoord.x,aTexCoord.y);
	float maskvalue = texture(DestTexture, TexCoord).x;
	gl_Position = MVP * vec4(aPos, (maskvalue * 2000.0) - 1.0, 1);
}