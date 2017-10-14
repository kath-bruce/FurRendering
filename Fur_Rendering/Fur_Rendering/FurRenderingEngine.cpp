#include "FurRenderingEngine.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
namespace FurRenderingEngine {
	//shader
	std::unordered_map<std::string, GLuint> shaders;

	//models
	//std::unordered_map<std::string, Model> models;
	std::vector<Model> models;

	//mvstack
	std::stack<glm::mat4> mvStack;

	//eye
	//at
	//up
	glm::vec3 eye(0.0f, 1.0f, 0.0f);
	glm::vec3 at(0.0f, 1.0f, -1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	//projection

	GLuint screenHeight = 600;
	GLuint screenWidth = 800;

	glm::mat4 projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), (float)screenWidth / (float)screenHeight, 1.0f, 150.0f);

	GLuint loadBitmap(char * fname)
	{
		GLuint texID;
		glGenTextures(1, &texID); // generate texture ID

								  // load file - using core SDL library
		SDL_Surface *tmpSurface;
		tmpSurface = SDL_LoadBMP(fname);
		if (!tmpSurface) {
			std::cout << "Error loading bitmap" << std::endl;
		}

		// bind texture and set parameters
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		SDL_PixelFormat *format = tmpSurface->format;

		GLuint externalFormat, internalFormat;
		if (format->Amask) {
			internalFormat = GL_RGBA;
			externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
		}
		else {
			internalFormat = GL_RGB;
			externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
			externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
		return texID;	// return value of texture ID
	}

	void addModel(char * modelFileName, char * textureFileName, glm::vec3 pos, glm::vec3 scale, GLfloat r, std::string modelName, std::string shaderName)
	{
		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (addModel): " << shaderName << " has not been initialised!\n";
			return;
		}

		GLuint shaderProgram = shaders[shaderName];

		std::vector<GLfloat> verts;
		std::vector<GLfloat> norms;
		std::vector<GLfloat> tex_coords;
		std::vector<GLuint> indices;

		rt3d::loadObj(modelFileName, verts, norms, tex_coords, indices);

		GLuint meshIndexCount = indices.size();
		GLuint modelObj = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());
		GLuint texture = loadBitmap(textureFileName);


		//models.insert({ modelName, Model(modelObj, texture, pos, scale, r, meshIndexCount, shaderProgram) });
		models.push_back(Model(modelObj, texture, pos, scale, r, meshIndexCount, shaderProgram));

	}

	void addShader(std::string shaderName, const char * vert, const char * frag)
	{
		GLuint shaderProgram = rt3d::initShaders(vert, frag);
		shaders.insert({ shaderName, shaderProgram });
	}

	void setLight(std::string shaderName, rt3d::lightStruct light)
	{
		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (setLight): " << shaderName << " has not been initialised!\n";
			return;
		}

		GLuint shaderProgram = shaders[shaderName];
		rt3d::setLight(shaderProgram, light);
	}

	void setMaterial(std::string shaderName, rt3d::materialStruct mat)
	{
		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (setMaterial): " << shaderName << " has not been initialised!\n";
			return;
		}

		GLuint shaderProgram = shaders[shaderName];
		rt3d::setMaterial(shaderProgram, mat);
	}

	void setUniform(std::string shaderName, std::function<void(GLuint)> lambda)
	{
		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (setUniform): " << shaderName << " has not been initialised!\n";
			return;
		}

		GLuint shaderProgram = shaders[shaderName];

		lambda(shaderProgram);
	}

	

	/*void updateModelPosScaleRot(std::string modelName, glm::vec3 pos, glm::vec3 scale, GLfloat rot)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (updateModelPosScaleRot): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models[modelName];

		m.setPos(pos);
		m.setScale(scale);
		m.setRot(rot);
	}*/


	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
	}

	void draw()
	{
		//for each model in models vector - draw

		glEnable(GL_CULL_FACE);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 modelview(1.0); // set base position for scene
		mvStack.push(modelview);

		at = moveForward(eye, 0.0f, 1.0f);
		mvStack.top() = glm::lookAt(eye, at, up);

		for (auto m : models)
		{
			glUseProgram(m.getShaderProgram());
			rt3d::setUniformMatrix4fv(m.getShaderProgram(), "projection", glm::value_ptr(projection));

			glBindTexture(GL_TEXTURE_2D, m.getTexture());

			mvStack.push(mvStack.top());

			mvStack.top() = glm::translate(mvStack.top(), m.getPos());

			mvStack.top() = glm::rotate(mvStack.top(), float(m.getRot() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
			
			mvStack.top() = glm::scale(mvStack.top(), m.getScale());

			rt3d::setUniformMatrix4fv(m.getShaderProgram(), "modelview", glm::value_ptr(mvStack.top()));

			rt3d::drawIndexedMesh(m.getModel(), m.getMeshIndexCount(), GL_TRIANGLES);

			mvStack.pop();
		}

		/*for (auto m : models)
		{
			glUseProgram(m.second.getShaderProgram());
			rt3d::setUniformMatrix4fv(m.second.getShaderProgram(), "projection", glm::value_ptr(projection));

			glBindTexture(GL_TEXTURE_2D, m.second.getTexture());

			mvStack.push(mvStack.top());

			mvStack.top() = glm::translate(mvStack.top(), m.second.getPos());

			mvStack.top() = glm::rotate(mvStack.top(), float(m.second.getRot() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));

			mvStack.top() = glm::scale(mvStack.top(), m.second.getScale());

			rt3d::setUniformMatrix4fv(m.second.getShaderProgram(), "modelview", glm::value_ptr(mvStack.top()));

			rt3d::drawIndexedMesh(m.second.getModel(), m.second.getMeshIndexCount(), GL_TRIANGLES);

			mvStack.pop();
		}*/

		// remember to use at least one pop operation per push...
		mvStack.pop(); // initial matrix

	}
}