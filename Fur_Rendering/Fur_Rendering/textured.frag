// textured.frag
#version 330

// Some drivers require the following
precision highp float;

uniform sampler2D textureUnit0;

in vec2 ex_TexCoord;
//in vec4 proj_vertex;
layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour
	//vec4 reflectTexCoord= proj_vertex/proj_vertex.w;//perspective division
	//reflectTexCoord=(reflectTexCoord+1)*(0.5);//remapping

	//out_Color = texture(textureUnit0, reflectTexCoord.xy);
	out_Color = texture(textureUnit0, ex_TexCoord);
}