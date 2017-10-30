#pragma once


#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include "Model.h"

namespace FurRenderingEngine {
	#define DEG_TO_RADIAN 0.017453293
	#define SCREENWIDTH 800
	#define SCREENHEIGHT 600
	#define FUR_TEXTURE_DIMENSION 512
	
	void addShader(std::string shaderName, const char * vert, const char * frag);

	void regenTexture(std::string modelName);

	void addModel(const char * modelFileName, glm::vec3 pos,
		glm::vec3 scale, std::string modelName, std::string shaderName);

	//set light - for later
	void setLight(std::string shaderName, rt3d::lightStruct light);

	//set material - for later
	void setMaterial(std::string shaderName, rt3d::materialStruct mat);

	void setUniform(std::string shaderName, std::function<void(GLuint)> lambda); 

	void updateModelRot(std::string modelName, GLfloat rotX, GLfloat rotY, GLfloat rotZ);

	void draw();

	void resetModelRot(std::string modelName);

	void setGravityEffect(int grav_effect);

	void setNumLayers(int num_l);

	void setFurChance(int fur_c);
}

