#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Model
{
private:

	GLuint model;
	GLuint texture;
	glm::vec3 pos;
	glm::vec3 scale;
	GLfloat rotX;
	GLfloat rotY;
	GLfloat rotZ;
	GLuint meshIndexCount;
	GLuint shaderProgram;

	//glm::quat rot;

public:
	Model(GLuint m, GLuint t, glm::vec3 p, glm::vec3 s, GLuint mIC, GLuint sP);

	GLuint getModel() { return model; }

	GLuint getTexture() { return texture; }
	void setTexture(GLuint t) { texture = t; }

	glm::vec3 getPos() { return pos; }
	void setPos(glm::vec3 newPos) { pos = newPos; }

	glm::vec3 getScale() { return scale; }
	void setScale(glm::vec3 newScale) { scale = newScale;}

	GLfloat getRotX() { return rotX; }
	void setRotX(GLfloat newRotX) { rotX = newRotX; 
		if (rotX >= 360.0f)
		{ rotX = 0.0f; }
		else if (rotX < 0.0f)
		{ rotX = 360.0f; }
	}

	GLfloat getRotY() { return rotY; }
	void setRotY(GLfloat newRotY) { rotY = newRotY;
		if (rotY >= 360.0f)
		{ rotY = 0.0f; }
		else if (rotY < 0.0f)
		{ rotY = 360.0f; }
	}

	GLfloat getRotZ() { return rotZ; }
	void setRotZ(GLfloat newRotZ) { rotZ = newRotZ;
		if (rotZ >= 360.0f)
		{ rotZ = 0.0f; }
		else if (rotZ < 0.0f)
		{ rotZ = 360.0f; }
	}

	GLuint getMeshIndexCount() { return meshIndexCount; }
	GLuint getShaderProgram() { return shaderProgram; }

	~Model();
};

