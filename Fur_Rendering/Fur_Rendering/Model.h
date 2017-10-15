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

public:
	Model(GLuint m, GLuint t, glm::vec3 p, glm::vec3 s, GLuint mIC, GLuint sP);

	GLuint getModel() { return model; }
	GLuint getTexture() { return texture; }

	glm::vec3 getPos() { return pos; }
	void setPos(glm::vec3 newPos) { pos = newPos; }

	glm::vec3 getScale() { return scale; }
	void setScale(glm::vec3 newScale) { scale = newScale; }

	GLfloat getRotX() { return rotX; }
	void setRotX(GLfloat newRotX) { rotX = newRotX; }

	GLfloat getRotY() { return rotY; }
	void setRotY(GLfloat newRotY) { rotY = newRotY; }

	GLfloat getRotZ() { return rotZ; }
	void setRotZ(GLfloat newRotZ) { rotZ = newRotZ; }

	GLuint getMeshIndexCount() { return meshIndexCount; }
	GLuint getShaderProgram() { return shaderProgram; }

	~Model();
};

