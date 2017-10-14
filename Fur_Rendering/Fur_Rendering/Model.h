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
	GLfloat rot;
	GLuint meshIndexCount;
	GLuint shaderProgram;

public:
	Model(GLuint m, GLuint t, glm::vec3 p, glm::vec3 s, GLfloat r, GLuint mIC, GLuint sP);

	GLuint getModel() { return model; }
	GLuint getTexture() { return texture; }

	glm::vec3 getPos() { return pos; }
	void setPos(glm::vec3 newPos) { pos = newPos; }

	glm::vec3 getScale() { return scale; }
	void setScale(glm::vec3 newScale) { scale = newScale; }

	GLfloat getRot() { return rot; }
	void setRot(GLfloat newRot) { rot = newRot; }

	GLuint getMeshIndexCount() { return meshIndexCount; }
	GLuint getShaderProgram() { return shaderProgram; }

	~Model();
};

