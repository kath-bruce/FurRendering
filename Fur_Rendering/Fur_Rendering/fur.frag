// textured.frag
#version 330

// Some drivers require the following
precision highp float;

uniform sampler2D textureUnit0;

in vec2 ex_TexCoord;
in float ex_fragLayer;
//in vec3 ex_Normal;
in vec4 ex_furColour;

layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour

	float fakeShadow = mix(0.2, 0.9, (ex_fragLayer/20.0));
	vec4 furData = texture(textureUnit0, ex_TexCoord);
	vec4 furColour = ex_furColour * fakeShadow;

	float visibility = (ex_fragLayer > (furData.r*20)) ? 0.0 : furData.a;

	visibility = visibility - (ex_fragLayer/100.0);

	furColour.a = visibility; //(ex_fragLayer == 0.0) ? 0.0 : visibility;

	out_Color = furColour;
	//out_Color = texture(textureUnit0, ex_TexCoord);
}