#version 430
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 colour;
layout (location = 2) in float Size;
out vec3 Colour;


void main()
{
    gl_Position = vec4(aPos,0, 1);
	Colour = colour;
	gl_PointSize = Size * 2;
}
