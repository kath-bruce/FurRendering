#pragma once


#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <SDL_ttf.h>
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include "Model.h"

namespace FurRenderingEngine {
	#define DEG_TO_RADIAN 0.017453293
	#define SCREENWIDTH 800
	#define SCREENHEIGHT 600
	#define FUR_TEXTURE_DIMENSION 512
	#define FUR_LAYERS 80
	
	void addShader(std::string shaderName, const char * vert, const char * frag);

	void regenTexture(std::string modelName);

	void addModel(const char * modelFileName, glm::vec3 pos,
		glm::vec3 scale, GLfloat r, std::string modelName, std::string shaderName);

	//set light
	void setLight(std::string shaderName, rt3d::lightStruct light);

	//set material
	void setMaterial(std::string shaderName, rt3d::materialStruct mat);

	//this should work so show it off somehow
	void setUniform(std::string shaderName, std::function<void(GLuint)> lambda); 

	void updateModelRot(std::string modelName, GLfloat rotX, GLfloat rotY, GLfloat rotZ);

	void draw();
}

