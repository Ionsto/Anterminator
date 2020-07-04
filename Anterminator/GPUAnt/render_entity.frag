#version 430
out vec4 FragColor;
in vec3 Colour;  
//in vec2 texCoord;
uniform int UseDrawMask = 0;
uniform sampler2D DrawMask;
in vec2 DrawMaskLocation;

void main()
{
//	vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    //float sqRadius = dot(circCoord, circCoord);
    //float value = 2.0 * (1.0 - sqRadius);
    //float alpha = smoothstep(0.0, 1.0, value);
	vec2 tCoord = gl_PointCoord * 2.0 - vec2(1.0, 1.0);
    float sqRadius = tCoord.x * tCoord.x + tCoord.y * tCoord.y;
    float value = 2.0 * (1.0 - sqRadius);
    float alpha = smoothstep(0.9, 1.0, value);
    FragColor = vec4(Colour,alpha);
	if(alpha == 0){
		discard;
	}
	if(UseDrawMask == 1){
		if(texture(DrawMask,DrawMaskLocation).x < 0.5){
			discard;
		}
	}
}
