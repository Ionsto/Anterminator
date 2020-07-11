#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D DestTexture;

const float MaskResolution = 8;
const int WorldSize = 2048;
const int MaskCount = int(2 * WorldSize / MaskResolution);

void main()
{
	float maskvalue = texture(DestTexture, TexCoord).x;
	vec3 c = vec3(0,0,0);
	if(maskvalue > 0.0001){
		c = vec3(1,1,1) * maskvalue + vec3(0.2,0.2,0.2);
	}
	else{
		c = vec3(0,0,0);
	}

    FragColor = vec4(c,1);
}
