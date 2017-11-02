// fur_tut.frag
#version 330

// Some drivers require the following
precision highp float;

uniform sampler2D textureUnit0;

in vec2 ex_TexCoord;
in float ex_fragLayer;
in vec4 ex_furColour;
layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour
	vec4 furData = texture(textureUnit0, ex_TexCoord);
	float fakeShadow = mix(0.4, 1.0, (ex_fragLayer/15.0));
	vec4 furColour = ex_furColour * fakeShadow;

	if (ex_fragLayer > furData.a*20)
	{
		furData.a = 0.0;
	}
	else
	{
		furData.a = 1.0 - (ex_fragLayer/100.0);
	}

	furColour.a = furData.a;

	out_Color = furColour;
}