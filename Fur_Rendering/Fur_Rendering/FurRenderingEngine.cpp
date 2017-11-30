#include "FurRenderingEngine.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>
namespace FurRenderingEngine {
#define SKYBOX "skybox"

	//shader
	std::unordered_map<std::string, GLuint> shaders;

	//models
	std::unordered_map<std::string, Model> models;

	//mvstack
	std::stack<glm::mat4> mvStack;

	//eye	- camera
	//at	- point camera initially faces
	//up	- unit up vector
	glm::vec3 eye(0.0f, 5.0f, 5.0f);
	glm::vec3 at(0.0f, 1.0f, -3.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 camDiff(0.0f, 5.0f, 5.0f);

	//projection
	glm::mat4 projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), (float)SCREENWIDTH / (float)SCREENHEIGHT, 1.0f, 150.0f);

	//time - used for swaying effect
	float t = 0.0f;

	//used during fur texture generation
	int fur_chance = 30;

	GLubyte furTexture[FUR_TEXTURE_DIMENSION][FUR_TEXTURE_DIMENSION][4];

	//skybox
	GLuint skybox[5];
	GLuint cube;
	GLuint meshIndexCount;
	
	//Theta
	float theta = 0.0f;

	//Light
	rt3d::lightStruct light0 = {
		{ 0.4f, 0.4f, 0.4f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
		{ -5.0f, 2.0f, 2.0f, 1.0f }  // position
	};
	glm::vec4 lightPos(-5.0f, 2.0f, 2.0f, 1.0f); //light position

	//Normal Map Material
	rt3d::materialStruct material1 = {
		{ 0.4f, 0.4f, 1.0f, 1.0f }, // ambient
		{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
		{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
		1.0f  // shininess
	};

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
				//generate an integer between 1 and 100
				int furPoint = rand() % 100 + 1;

				if (furPoint > fur_chance) {
					furTexture[i][j][0] = (GLubyte)0;		//r
					furTexture[i][j][1] = (GLubyte)0;		//g
					furTexture[i][j][2] = (GLubyte)0;		//b
					furTexture[i][j][3] = (GLubyte)0;		//a
				}
				else
				{
					furTexture[i][j][0] = (GLubyte)255;		//r
					furTexture[i][j][1] = (GLubyte)255;		//g
					furTexture[i][j][2] = (GLubyte)255;		//b
					furTexture[i][j][3] = (GLubyte)255;		//a
				}

			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FUR_TEXTURE_DIMENSION,
			FUR_TEXTURE_DIMENSION, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			furTexture);

		glGenerateMipmap(GL_TEXTURE_2D);

		return texID;	// return value of texture ID
	}

	GLuint loadCubeMap(const char *fname[6], GLuint *texID)
	{
		glGenTextures(1, texID); // generate texture ID
		GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };
		SDL_Surface *tmpSurface;

		glBindTexture(GL_TEXTURE_CUBE_MAP, *texID); // bind texture and set parameters
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (int i = 0; i < 6; i++)
		{
			// load file - using core SDL library
			tmpSurface = SDL_LoadBMP(fname[i]);
			if (!tmpSurface)
			{
				std::cout << "Error loading bitmap" << std::endl;
				return *texID;
			}

			glTexImage2D(sides[i], 0, GL_RGB, tmpSurface->w, tmpSurface->h, 0,
				GL_BGR, GL_UNSIGNED_BYTE, tmpSurface->pixels);
			// texture loaded, free the temporary buffer
			SDL_FreeSurface(tmpSurface);
		}
		return *texID;	// return value of texure ID, redundant really
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

	void addSkybox(const char *fname[6], const char * skyboxVert, const char * skyboxFrag)
	{

		GLuint skyboxProg = rt3d::initShaders(skyboxVert, skyboxFrag);
		shaders.insert({ SKYBOX, skyboxProg });

		std::vector<GLfloat> verts;
		std::vector<GLfloat> norms;
		std::vector<GLfloat> tex_coords;
		std::vector<GLuint> indices;

		rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);

		meshIndexCount = indices.size();

		cube = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());;

		loadCubeMap(fname, &skybox[0]);
	}

	void calculateTangents(std::vector<GLfloat> &tangents, std::vector<GLfloat> &verts, std::vector<GLfloat> &normals,
		std::vector<GLfloat> &tex_coords, std::vector<GLuint> &indices) {

		// Code taken from http://www.terathon.com/code/tangent.html and modified slightly to use vectors instead of arrays
		// Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. 

		std::vector<glm::vec3> tan1(verts.size() / 3, glm::vec3(0.0f));
		std::vector<glm::vec3> tan2(verts.size() / 3, glm::vec3(0.0f));
		int triCount = indices.size() / 3;
		for (int c = 0; c < indices.size(); c += 3)
		{
			int i1 = indices[c];
			int i2 = indices[c + 1];
			int i3 = indices[c + 2];

			glm::vec3 v1(verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2]);
			glm::vec3 v2(verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2]);
			glm::vec3 v3(verts[i3 * 3], verts[i3 * 3 + 1], verts[i3 * 3 + 2]);

			glm::vec2 w1(tex_coords[i1 * 2], tex_coords[i1 * 2 + 1]);
			glm::vec2 w2(tex_coords[i2 * 2], tex_coords[i2 * 2 + 1]);
			glm::vec2 w3(tex_coords[i3 * 2], tex_coords[i3 * 2 + 1]);

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		for (int a = 0; a < verts.size(); a += 3)
		{
			glm::vec3 n(normals[a], normals[a + 1], normals[a + 2]);
			glm::vec3 t = tan1[a / 3];

			glm::vec3 tangent;
			tangent = (t - n * glm::normalize(glm::dot(n, t)));

			// handedness
			GLfloat w = (glm::dot(glm::cross(n, t), tan2[a / 3]) < 0.0f) ? -1.0f : 1.0f;

			tangents.push_back(tangent.x);
			tangents.push_back(tangent.y);
			tangents.push_back(tangent.z);
			tangents.push_back(w);

		}
	}

	void addModel(const char * modelFileName, glm::vec3 pos, glm::vec3 scale, std::string modelName, 
		std::string shaderName, bool hasNormalMapping, const char * textureFileName, int num_layers)
	{
		//only shaders added to the engine are allowed to be attached to models as shaders should be written for fur rendering
		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (addModel): " << shaderName << " has not been initialised!\n";
			return;
		}

		//this is a small check to ensure that the same model is not repeatedly loaded from file
		//passing a different model name will allow another copy to be loaded if needed
		if (models.count(modelName) > 1)
		{
			std::cout << "ERROR (addModel): " << modelName << " has already been loaded!\n";
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

		if (hasNormalMapping)
		{
			std::vector<GLfloat> tangents;
			calculateTangents(tangents, verts, norms, tex_coords, indices);
			// And add the tangent data
			glBindVertexArray(modelObj);
			GLuint VBO;
			glGenBuffers(1, &VBO);
			// VBO for tangent data
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(GLfloat), tangents.data(), GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(5);
			glBindVertexArray(0);
		}

		GLuint texture;

		if (textureFileName == "")
		{
			texture = generateTexture();
		}
		else
		{
			texture = loadBitmap(textureFileName);
		}

		models.emplace(std::make_pair(modelName, Model(modelObj, texture, pos, scale, meshIndexCount, shaderProgram, num_layers)));

	}

	void regenTexture(std::string modelName)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (regenTexture): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);
		GLuint text = generateTexture();

		m.setTexture(text);

		//this is to ensure the model with the new texture is the only version of the model
		models.insert_or_assign(modelName, m);
	}

	void addShader(std::string shaderName, const char * vert, const char * frag)
	{
		GLuint shaderProgram = rt3d::initShaders(vert, frag);
		shaders.insert({ shaderName, shaderProgram });
	}

	void setMaterial(std::string shaderName, rt3d::materialStruct mat)
	{
		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (setMaterial): " << shaderName << " has not been initialised!\n";
			return;
		}

		GLuint shaderProgram = shaders[shaderName];

		glUseProgram(shaderProgram);

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

		glUseProgram(shaderProgram);

		lambda(shaderProgram);
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

		//this is to ensure the model with the new rotation is the only version of the model
		models.insert_or_assign(modelName, m);
	}

	void updateModelPos(std::string modelName, GLfloat diffX, GLfloat diffY, GLfloat diffZ)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (updateModelPos): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);

		glm::vec3 oldPos = m.getPos();
		glm::vec3 newPos(m.getPos().x + diffX, m.getPos().y + diffY, m.getPos().z + diffZ);
		m.setPos(newPos);

		bool collision = false;
		std::string collectableId = "";

		for (auto mod : models)
		{
			if (mod.first != modelName && CollisionDetector::detectCollision(mod.second, m))
			{
				m.setPos(oldPos);
				collision = true;

				if (mod.first.substr(0, 11) == "collectable")
				{
					std::cout << "deleting collectable " << mod.first.substr(11,12) << "\n";
					collectableId = mod.first;
				}
			}
		}

		if (collectableId != "")
		{
			models.erase(collectableId);
		}

		//this is to ensure the model with the new position is the only version of the model
		if (!collision)
			models.insert_or_assign(modelName, m);

		at = m.getPos();
	}

	void zoomToModel(std::string modelName, GLfloat zoomFactor)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (zoomToModel): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);

		if (zoomFactor < 0.0f)
		{
			if (camDiff.y > 0.0f)
			{
				camDiff.y += zoomFactor;
			}
			else if (camDiff.z > 2.5f)
			{
				camDiff.z += zoomFactor;
			}
		}
		else
		{
			if (camDiff.z < 5.0f)
			{
				camDiff.z += zoomFactor;
			}
			else if (camDiff.y < 5.0f)
			{
				camDiff.y += zoomFactor;
			}
		}
		//eye
	}

	void draw()
	{
		//for each model in models vector - draw
		glClearColor(0.0f, 0.75f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 modelview(1.0); // set base position for scene
		mvStack.push(modelview);

		//Independently calculate the model matrix from the view for normal mapping uniform.
		glm::mat4 modelMatrix(1.0);
		mvStack.push(mvStack.top());
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 1.0f, -3.0f));
		modelMatrix = glm::rotate(modelMatrix, float(theta*DEG_TO_RADIAN), glm::vec3(1.0f, 1.0f, 1.0f));//transform theta to radians if expressed in degrees
		mvStack.top() = mvStack.top() * modelMatrix;

		eye = at + camDiff;
		mvStack.top() = glm::lookAt(eye, at, up);

		t += 0.01f;

		//Light position for normal mapping
		glm::vec4 tmp = mvStack.top()*lightPos;
		light0.position[0] = tmp.x;
		light0.position[1] = tmp.y;
		light0.position[2] = tmp.z;

		//skybox
		{
			glUseProgram(shaders.at(SKYBOX));
			rt3d::setUniformMatrix4fv(shaders.at(SKYBOX), "projection", glm::value_ptr(projection));

			glDepthMask(GL_FALSE); // make sure depth test is off
			glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
			mvStack.push(glm::mat4(mvRotOnlyMat3));

			glCullFace(GL_FRONT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
			mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
			rt3d::setUniformMatrix4fv(shaders.at(SKYBOX), "modelview", glm::value_ptr(mvStack.top()));
			rt3d::drawIndexedMesh(cube, meshIndexCount, GL_TRIANGLES);
			mvStack.pop();

			glCullFace(GL_FRONT);

			glDepthMask(GL_TRUE);
		}


		for (auto m : models)
		{
			glUseProgram(m.second.getShaderProgram());

			//this needs to be set once for every model every frame
			int uniformIndex = glGetUniformLocation(m.second.getShaderProgram(), "time");
			glUniform1f(uniformIndex, sin(t));

			for (int i = 0; i < m.second.getNumLayers(); i++)
			{

				//Projection uniform
				rt3d::setUniformMatrix4fv(m.second.getShaderProgram(), "projection", glm::value_ptr(projection));

				//this needs to be set for every layer
				uniformIndex = glGetUniformLocation(m.second.getShaderProgram(), "layer");
				glUniform1f(uniformIndex, i);

				//Camera pos uniform
				uniformIndex = glGetUniformLocation(m.second.getShaderProgram(), "cameraPos");
				glUniform3fv(uniformIndex, 1, glm::value_ptr(eye));

				//Light and Material uniforms
				rt3d::setLight(m.second.getShaderProgram(), light0);
				rt3d::setMaterial(m.second.getShaderProgram(), material1);

				glBindTexture(GL_TEXTURE_2D, m.second.getTexture());

				mvStack.push(mvStack.top());

				mvStack.top() = glm::translate(mvStack.top(), m.second.getPos());

				mvStack.top() = glm::rotate(mvStack.top(), float(m.second.getRotX() * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
				mvStack.top() = glm::rotate(mvStack.top(), float(m.second.getRotY() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
				mvStack.top() = glm::rotate(mvStack.top(), float(m.second.getRotZ() * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

				mvStack.top() = glm::scale(mvStack.top(), m.second.getScale());

				//Light position uniform
				rt3d::setLightPos(m.second.getShaderProgram(), glm::value_ptr(tmp));

				//Modelview uniform
				rt3d::setUniformMatrix4fv(m.second.getShaderProgram(), "modelview", glm::value_ptr(mvStack.top()));

				//Modelmatrix uniform
				rt3d::setUniformMatrix4fv(m.second.getShaderProgram(), "modelMatrix", glm::value_ptr(mvStack.top()));

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
			std::cout << "ERROR (resetModelRot): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);

		m.setRotX(0.0f);
		m.setRotY(0.0f);
		m.setRotZ(0.0f);

		//this is to ensure the model with the new rotation is the only version of the model
		models.insert_or_assign(modelName, m);
	}

	void setNumLayers(std::string modelName, int num_l)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (setNumLayers): " << modelName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);

		m.setNumLayers(num_l);

		//this is to ensure the model with the new texture is the only version of the model
		models.insert_or_assign(modelName, m);
	}

	void setFurChance(int fur_c)
	{
		fur_chance = fur_c;
	}

	GLuint getSkybox()
	{
		return skybox[0];
	}

	void setShader(std::string modelName, std::string shaderName)
	{
		if (models.count(modelName) < 1)
		{
			std::cout << "ERROR (setShader): " << modelName << " has not been initialised!\n";
			return;
		}

		if (shaders.count(shaderName) < 1)
		{
			std::cout << "ERROR (setShader): " << shaderName << " has not been initialised!\n";
			return;
		}

		Model m = models.at(modelName);

		m.setShaderProgam(shaders.at(shaderName));

		//this is to ensure the model with the new shader is the only version of the model
		models.insert_or_assign(modelName, m);

	}
}