// fur.frag
#version 330

// Some drivers require the following
precision highp float;

//fur texture
uniform sampler2D textureUnit0;

//colour texture
uniform sampler2D textureUnit1;

uniform int cutoffLayer; 
uniform vec4 fur_colour; //to be deprecated????

in vec2 ex_TexCoord;
in float ex_fragLayer;

layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour

	//gives a shadow effect based on the current layer
	//increasing the divisor in the third parameter will lighten the shadow
	//in smaller increments, effectively making the fur darker
	float fakeShadow = mix(0.4, 1.0, (ex_fragLayer/15.0));
	vec4 furColour = texture(textureUnit1, ex_TexCoord) * fakeShadow;

	vec4 furData = texture(textureUnit0, ex_TexCoord);

	//the cutoff point for when to decrease alpha
	//since furData.a will be a float between 0 and 1
	//if the alpha is 0, alpha will remain 0 at any layer
	//if the alpha is 1, 
	//the condition becomes - (if current layer > cutoffLayer)
	float visibility = (ex_fragLayer > (furData.a*cutoffLayer)) ? 0.0 : 1.0;

	//this is to decrease alpha based on layer and by how much
	visibility = visibility - (ex_fragLayer/100.0);

	furColour.a = visibility;

	out_Color = furColour;
}