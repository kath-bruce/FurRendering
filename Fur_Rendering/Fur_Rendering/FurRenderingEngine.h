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

	void addShader(std::string shaderName, const char * vert, const char * frag);


	void addModel(const char * modelFileName, const char * textureFileName, glm::vec3 pos,
		glm::vec3 scale, GLfloat r, std::string modelName, std::string shaderName);


	//set light
	void setLight(std::string shaderName, rt3d::lightStruct light);

	//set material
	void setMaterial(std::string shaderName, rt3d::materialStruct mat);

	void setUniform(std::string shaderName, std::function<void(GLuint)> lambda); 


	//void updateModelPosScaleRot(std::string modelName, glm::vec3 pos, glm::vec3 scale, GLfloat rot);

	//generateTexture
	
	
	//
	void draw();
}

