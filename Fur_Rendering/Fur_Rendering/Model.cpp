#include "Model.h"



Model::Model(GLuint m, GLuint t, glm::vec3 p, glm::vec3 s, GLuint mIC, GLuint sP, int layers) 
{
	model = m;
	texture = t;
	pos = p;
	scale = s;

	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;

	numLayers = layers;

	meshIndexCount = mIC;
	shaderProgram = sP;
}

Model::~Model()
{
}
