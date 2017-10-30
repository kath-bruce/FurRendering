// fur.frag
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

	float fakeShadow = mix(0.4, 1.0, (ex_fragLayer/25.0));
	vec4 furData = texture(textureUnit0, ex_TexCoord);
	vec4 furColour = ex_furColour * fakeShadow;

	float visibility = (ex_fragLayer > (furData.r*20)) ? 0.0 : furData.a; //figure this out!!!

	visibility = visibility - (ex_fragLayer/100.0);

	furColour.a = visibility;

	out_Color = furColour;
}