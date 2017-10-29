#include "Model.h"



Model::Model(GLuint m, GLuint t, glm::vec3 p, glm::vec3 s, GLuint mIC, GLuint sP) 
{
	model = m;
	texture = t;
	pos = p;
	scale = s;

	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;

	

	meshIndexCount = mIC;
	shaderProgram = sP;
}

Model::~Model()
{
}
