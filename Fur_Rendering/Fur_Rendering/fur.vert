// fur.vert
#version 330

uniform mat4 modelview;
uniform mat4 projection;

uniform vec3 displacement;
uniform float layer;
uniform vec3 gravity;
uniform int gravity_effect;
uniform float time;

uniform vec4 fur_colour;

in  vec3 in_Position;
in  vec3 in_Normal;
in  vec2 in_TexCoord;

out vec2 ex_TexCoord;
out float ex_fragLayer;
out vec4 ex_furColour;

// multiply each vertex position by the MVP matrix
void main(void) {

	//displacing the current shell by the normal based on current layer
	vec3 shell = in_Position.xyz + in_Normal.xyz*layer*displacement;

	//displacing the shell to create a gravity effect
	float k = pow(layer/gravity_effect,2);
	shell = shell + (gravity*k);

	//displacing the shell to create a swaying effect
	shell.x += time*layer*0.004f;

	vec4 newPos = vec4(shell, 1.0);

	//'cut' the fur if it's too low
	if (newPos.y < -1.0)
		newPos.y = -1.0;

	gl_Position = (projection * modelview * newPos);

	ex_TexCoord = in_TexCoord;

	ex_fragLayer = layer;

	ex_furColour = fur_colour;

}