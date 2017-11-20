// Phong fragment shader phong-tex.frag matched with phong-tex.vert
#version 330

// Some drivers require the following
precision highp float;
uniform vec4 lightPosition[11];
uniform struct lightStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
}lights[11];

struct materialStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform materialStruct material;
uniform sampler2D textureUnit0;

uniform float attConst;
uniform float attLinear;
uniform float attQuadratic;


in vec3 ex_N;
in vec3 ex_V;
in vec2 ex_TexCoord;
in vec4 vertexPosition;

layout(location = 0) out vec4 out_Color;

vec4 CalculateLight(int index) {
	// Ambient intensity
	vec4 ambientI = lights[index].ambient * material.ambient;
	vec3 ex_L = vec3(0.0,0.0,0.0);
	if (lightPosition[index].w!=0.0) { ex_L = normalize(lightPosition[index].xyz - vertexPosition.xyz); } 
	else { ex_L = normalize(-lightPosition[index].xyz); }

	// Diffuse intensity
	vec4 diffuseI = lights[index].diffuse * material.diffuse;
	diffuseI = diffuseI * max(dot(normalize(ex_N),normalize(ex_L)),0);
	// Specular intensity
	// Calculate R - reflection of light
	vec3 R = normalize(reflect(normalize(-ex_L),normalize(ex_N)));

	vec4 specularI = lights[index].specular * material.specular;
	specularI = specularI * pow(max(dot(R,ex_V),0), material.shininess);
   
   float ex_D = distance(vertexPosition,lightPosition[index]);
	float attenuation= (lightPosition[index].w!=0.0 ? (1.0f/(attConst + attLinear * ex_D + attQuadratic * ex_D*ex_D)) : 1.0);
	vec4 tmp_Color = (diffuseI + specularI)*texture(textureUnit0, ex_TexCoord);
	//Attenuation does not affect transparency
	vec4 litColour = vec4(tmp_Color.rgb * attenuation, tmp_Color.a);
	vec4 amb=min(ambientI,vec4(1.0f));
	return litColour+amb;
}

void main(void) { 
	vec4 result = vec4(0.0,0.0,0.0,1.0);
	for(int i = 0; i < 11; i++) {
		result += CalculateLight(i);
	}
	out_Color=min(result*texture(textureUnit0, ex_TexCoord),vec4(1.0f));
}