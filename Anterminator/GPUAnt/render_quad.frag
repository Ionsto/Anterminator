#version 330 core
out vec4 FragColor;
  
in vec3 TexCoord;

uniform sampler3D DestTexture;

void main()
{
	if(TexCoord.z >= 0){
		FragColor = vec4(length(texture(DestTexture, TexCoord).xy)/100.0,0,0,1);
	}
	else{
		float prey = length(texture(DestTexture, vec3(TexCoord.xy,0)).xy)/100.0;
		//FragColor = vec4(prey,0,0,1);
		float zspace = abs(TexCoord.z);
		float pred = length(texture(DestTexture, vec3(TexCoord.xy,zspace)).xy)/100.0;
		pred += length(texture(DestTexture, vec3(TexCoord.xy,zspace * 3.0)).xy)/100.0;
		float ant = length(texture(DestTexture, vec3(TexCoord.xy,zspace*2.0)).xy)/100.0;
		FragColor = vec4(prey,pred,ant,1);
	}
	//FragColor = vec4(1,1,1,1);
}
