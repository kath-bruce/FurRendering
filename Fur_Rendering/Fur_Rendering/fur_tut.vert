// fur_tut.vert
// use textures, but no lighting
#version 330

uniform mat4 modelview;
uniform mat4 projection;

uniform vec3 displacement;
uniform float layer;
out float ex_fragLayer;

in  vec3 in_Position;
in  vec3 in_Normal;

in vec2 in_TexCoord;
out vec2 ex_TexCoord;

// multiply each vertex position by the MVP matrix
void main(void) {

	vec3 shell = in_Position + in_Normal*layer*displacement;
	
	// vertex into eye coordinates
	vec4 vertexPosition = modelview * vec4(shell,1.0);
    gl_Position = projection * vertexPosition;

	ex_TexCoord = in_TexCoord;
	ex_fragLayer = layer;
}