// textured.vert
// use textures, but no lighting
#version 330

uniform mat4 modelview;
uniform mat4 projection;

uniform vec3 displacement;

uniform float layer;

in  vec3 in_Position;
//in  vec3 in_Colour; // colour not used with lighting
//in  vec3 in_Normal;
//out vec4 ex_Color;

in vec2 in_TexCoord;

out vec2 ex_TexCoord;

out float ex_fragLayer;

// multiply each vertex position by the MVP matrix
void main(void) {

	vec3 layerDisplacement = pow(layer, 2.0) * displacement;
	vec4 newPos = vec4(in_Position + layerDisplacement, 1.0);

	// vertex into eye coordinates
	//vec4 vertexPosition = modelview * vec4(in_Position,1.0);
    //gl_Position = projection * vertexPosition;

	gl_Position = projection * modelview * newPos;


	ex_TexCoord = in_TexCoord;

	ex_fragLayer = layer;
}