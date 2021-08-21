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
	const int Steps = 2;
	const float Res = 0.5/float(MaskCount);
	const float falloff = 10;
	for(int x = -Steps;x <= Steps;++x){
		for(int y = -Steps;y <= Steps;++y){
//			float dist = pow((x*x)+(y*y),0.2);
//			maskvalue += texture(DestTexture, TexCoord + (vec2(x,y) * Res)).x * (1.0/(1.0+dist));
			maskvalue += texture(DestTexture, TexCoord + (vec2(x,y) * Res)).x * 0.1;
		}
	}
//	maskvalue /= Steps * Steps;
//	maskvalue = clamp(maskvalue,0,1);
    FragColor = vec4(1,1,1,clamp(1-maskvalue,0,1));
	//FragColor = vec4(1,1,1,1);
}
