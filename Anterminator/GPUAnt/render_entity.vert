#version 430
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 colour;
layout (location = 2) in float Size;
layout (location = 3) uniform vec2 CameraPosition;
layout (location = 4) uniform vec2 Scale;
layout (location = 5) uniform float PixelScale;
out vec3 Colour;
out vec2 DrawMaskLocation;
uniform float WorldSize;

void main()
{
	vec2 worlddiff = (aPos - CameraPosition);
	for(int i = 0;i < 2;++i)
	{
		if(worlddiff[i] < -WorldSize){
			worlddiff[i] += 2* WorldSize;
		}
		if(worlddiff[i] > WorldSize){
			worlddiff[i] -= 2 * WorldSize;
		}

	}	
    gl_Position = vec4(worlddiff * Scale,0, 1);
	Colour = colour.xyz;
	gl_PointSize = Size * PixelScale;
	DrawMaskLocation = (aPos + WorldSize) / (WorldSize * 2);
}
