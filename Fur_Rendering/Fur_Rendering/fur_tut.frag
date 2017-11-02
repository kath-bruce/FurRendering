// fur_tut.frag
#version 330

// Some drivers require the following
precision highp float;

uniform sampler2D textureUnit0;

in vec2 ex_TexCoord;
in float ex_fragLayer;
layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour
	vec4 furData = texture(textureUnit0, ex_TexCoord);

	if (ex_fragLayer > furData.a*20)
	{
		furData.a = 0.0;
	}
	else
	{
		furData.a = 1.0 - (ex_fragLayer/100.0);
	}
	
	//float visibility = (ex_fragLayer > (furData.a*20)) ? 0.0 : 1.0;

	//visibility = visibility - (ex_fragLayer/100.0);

	//furData.a = visibility;

	out_Color = furData;
}