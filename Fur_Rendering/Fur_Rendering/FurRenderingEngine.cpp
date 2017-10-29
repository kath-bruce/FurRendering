#include "FurRenderingEngine.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
namespace FurRenderingEngine {
	//shader
	std::unordered_map<std::string, GLuint> shaders;

	//models
	std::unordered_map<std::string, Model> models;

	//mvstack
	std::stack<glm::mat4> mvStack;

	//eye
	//at
	//up
	glm::vec3 eye(0.0f, 1.0f, 0.0f);
	glm::vec3 at(0.0f, 1.0f, -1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	//projection

	glm::mat4 projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), (float)SCREENWIDTH / (float)SCREENHEIGHT, 1.0f, 150.0f);

	glm::vec3 gravity;

	float t = 0.0f;

	GLubyte furTexture[FUR_TEXTURE_DIMENSION][FUR_TEXTURE_DIMENSION][4];

	GLuint generateTexture()
	{
		GLuint texID;
		glGenTextures(1, &texID);

		// bind texture and set parameters
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		srand(time(NULL));

		for (int i = 0; i < FUR_TEXTURE_DIMENSION; i++)
		{
			for (int j = 0; j < FUR_TEXTURE_DIMENSION; j++)
			{
				int furChance = rand() % 100 + 1;

				if (furChance > 30) {
					furTexture[i][j][0] = (GLubyte)0;		//r
					furTexture[i][j][1] = (GLubyte)0;		//g
					furTexture[i][j][2] = (GLubyte)0;		//b
				}
				else
				{
					furTexture[i][j][0] = (GLubyte)255;		//r
					furTexture[i][j][1] = (GLubyte)255;		//g
					furTexture[i][j][2] = (GLubyte)255;		//b
				}


				furTexture[i][j][3] = (GLubyte)255;		//a
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FUR_TEXTURE_DIMENSION, 
			FUR_TEXTURE_DIMENSION, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			furTexture);

		glGenerateMipmap(GL_TEXTURE_2D);

		return texID;	// return value of texture ID
	}

	GLuint loadBitmap(const char * fname)
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

	void addModel(const char * modelFileName, glm::vec3 pos, glm::vec3 scale, std::string modelName, std::string shaderName)
	{
		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (addModel): " << shaderName << " has not been initialised!\n";
			return;
		}

		GLuint shaderProgram = shaders[shaderName];

		//todo: check if model or texture has already been loaded?

		std::vector<GLfloat> verts;
		std::vector<GLfloat> norms;
		std::vector<GLfloat> tex_coords;
		std::vector<GLuint> indices;

		rt3d::loadObj(modelFileName, verts, norms, tex_coords, indices);

		GLuint meshIndexCount = indices.size();

		//todo: error checking i.e., if tex_coords.size() < 0 pass nullptr instead?
		GLuint modelObj = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());
		GLuint texture = generateTexture();

		models.emplace(std::make_pair( modelName, Model(modelObj, texture, pos, scale, meshIndexCount, shaderProgram) ));

	}

	void regenTexture(std::string modelName)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (updateModelPosScaleRot): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);
		GLuint text = generateTexture();

		m.setTexture(text);

		models.insert_or_assign(modelName, m);
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

	void calcModelGravity(Model m)
	{
		//GLfloat modelRotX = m.getRotX();
		GLfloat modelRotZ = m.getRotZ();



		if (modelRotZ <= 180.0f) {
			GLfloat rotOver90 = modelRotZ / 90.0f;

			GLfloat productWithAbsValue = rotOver90 * abs(gravity.y);

			GLfloat subtractAbsValue = productWithAbsValue - abs(gravity.y);

			//subtractAbsValue == newGrav.y
			//newGrav.x == (gravity.y - subtractAbsValue)

			GLfloat newX = gravity.y - subtractAbsValue;

			glm::vec3 newGrav(newX, subtractAbsValue, 0.0);

			//pass in new gravity!!! but keep old
			int uniformIndex = glGetUniformLocation(m.getShaderProgram(), "gravity");
			glUniform3fv(uniformIndex, 1, glm::value_ptr(newGrav));

			std::cout << "gravity: " << newGrav.x << ", " << newGrav.y << ", 0\n";
		}
		else
		{
			GLfloat rotOver180 = modelRotZ / 180.0f;

			GLfloat productWithAbsValue = rotOver180 * (abs(gravity.y) * 2);

			GLfloat subtractAbsValue = productWithAbsValue - (abs(gravity.y) * 2);

			GLfloat reSubtractAbsValue = subtractAbsValue - (abs(gravity.y) * 2);

			//reSubtractAbsValue == newGrav.x
			GLfloat newY = gravity.y - reSubtractAbsValue;

			glm::vec3 newGrav(reSubtractAbsValue, newY, 0.0);

			//pass in new gravity!!! but keep old
			int uniformIndex = glGetUniformLocation(m.getShaderProgram(), "gravity");
			glUniform3fv(uniformIndex, 1, glm::value_ptr(newGrav));

			std::cout << "gravity: " << newGrav.x << ", " << newGrav.y << ", 0\n";
		}


	}

	void updateModelRot(std::string modelName, GLfloat rotX, GLfloat rotY, GLfloat rotZ)
	{

		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (updateModelRot): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);

		m.setRotX(m.getRotX() + rotX);
		m.setRotY(m.getRotY() + rotY);
		m.setRotZ(m.getRotZ() + rotZ);

		models.insert_or_assign(modelName, m);

		/*std::cout << "rot z: " << m.getRotZ() << std::endl;

		if (rotX != 0.0f || rotZ != 0.0f)
		{
			calcModelGravity(m);
		}*/
	}

	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
	}

	void draw()
	{
		//for each model in models vector - draw
		glClearColor(0.0f, 0.75f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 modelview(1.0); // set base position for scene
		mvStack.push(modelview);

		at = moveForward(eye, 0.0f, 1.0f);
		mvStack.top() = glm::lookAt(eye, at, up);

		t += 0.01f;

		for (auto m : models)
		{
			for (int i = 0; i < FUR_LAYERS; i++)
			{
				glUseProgram(m.second.getShaderProgram());
				rt3d::setUniformMatrix4fv(m.second.getShaderProgram(), "projection", glm::value_ptr(projection));

				int uniformIndex = glGetUniformLocation(m.second.getShaderProgram(), "layer");
				glUniform1f(uniformIndex, i);

				uniformIndex = glGetUniformLocation(m.second.getShaderProgram(), "time");
				glUniform1f(uniformIndex, sin(t));

				glBindTexture(GL_TEXTURE_2D, m.second.getTexture());

				mvStack.push(mvStack.top());

				mvStack.top() = glm::translate(mvStack.top(), m.second.getPos());

				mvStack.top() = glm::rotate(mvStack.top(), float(m.second.getRotX() * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
				mvStack.top() = glm::rotate(mvStack.top(), float(m.second.getRotY() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
				mvStack.top() = glm::rotate(mvStack.top(), float(m.second.getRotZ() * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

				mvStack.top() = glm::scale(mvStack.top(), m.second.getScale());

				rt3d::setUniformMatrix4fv(m.second.getShaderProgram(), "modelview", glm::value_ptr(mvStack.top()));

				rt3d::drawIndexedMesh(m.second.getModel(), m.second.getMeshIndexCount(), GL_TRIANGLES);

				mvStack.pop();
			}
		}

		// remember to use at least one pop operation per push...
		mvStack.pop(); // initial matrix

	}

	void resetModelRot(std::string modelName)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (updateModelRot): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);

		m.setRotX(0.0f);
		m.setRotY(0.0f);
		m.setRotZ(0.0f);

		models.insert_or_assign(modelName, m);

		//reset gravity
		int uniformIndex = glGetUniformLocation(m.getShaderProgram(), "gravity");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(gravity));

		std::cout << "gravity: " << gravity.x << ", " << gravity.y << ", 0\n";
	}

	void setGravity(glm::vec3 grav)
	{
		gravity = grav;
	}
}