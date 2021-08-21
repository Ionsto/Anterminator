#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 TexCoord;

uniform vec3 CameraPosition;
uniform vec2 Scale;
uniform float WorldSize;
uniform float Affiliation;
uniform mat4 MVP;

void main()
{
    //gl_Position = vec4((aPos + Offset) * Scale,1.0, 1.0);
//	vec2 worlddiff = (aPos - CameraPosition);
//	for(int i = 0;i < 2;++i)
//	{
//		if(worlddiff[i] < -WorldSize){
//			worlddiff[i] += 2* WorldSize;
//		}
//		if(worlddiff[i] > WorldSize){
//			worlddiff[i] -= 2 * WorldSize;
//		}
//	}	
//    gl_Position = vec4((aPos - CameraPosition.xy) * Scale,-CameraPosition.z, 1);
    gl_Position = MVP * vec4(aPos * WorldSize,-0.5, 1);
    TexCoord = vec3(aTexCoord.x,aTexCoord.y,Affiliation);
}