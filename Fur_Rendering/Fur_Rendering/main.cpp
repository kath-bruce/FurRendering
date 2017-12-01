#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//change 'false' to 'true' to get tutorial base
#define TUTORIAL_BASE false
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "FurRenderingEngine.h"
#include <SDL.h>

#define FUR_SHADER "fur_Shader"
#define FUR_OBJ "fur_Obj"
#define PLANE_SHADER "plane_Shader"
#define PLANE_OBJ "plane_Obj"
#define NORMAL_SHADER "normal_Shader"
#define NORMAL_OBJ "normal_Obj"
#define REFLECT_SHADER "reflect_shader"
#define REFLECT_OBJ "reflect_Obj"
#define REFRACT_SHADER "refract_shader"
#define REFRACT_OBJ "refract_Obj"

//the magnitude of the y value describes how intense the gravity is
int grav_effect = 60;

int layers = 60;
int cutoffLayer = 20;

int furAmount = 30;

//// light attenuation
float attConstant = 0.6f;
float attLinear = 0.1f;
float attQuadratic = 0.1f;

//reference: http://www.xbdev.net/directx3dx/specialX/Fur/
//reference: https://github.com/sdao/fur-demo
//reference: https://stackoverflow.com/questions/12969971/is-it-possible-to-manually-create-image-data-for-opengl-texture-use

// Set up rendering context
SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

													   // Create 800x600 window
	window = SDL_CreateWindow("Fur Render Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!TUTORIAL_BASE)
	{
		//shaders
		FurRenderingEngine::addShader(FUR_SHADER, "fur.vert", "fur.frag");
		FurRenderingEngine::addShader(PLANE_SHADER, "textured.vert", "textured.frag");
		FurRenderingEngine::addShader(NORMAL_SHADER, "normalmap.vert", "normalmap.frag");
		FurRenderingEngine::addShader(REFLECT_SHADER, "reflect.vert", "reflect.frag");
		FurRenderingEngine::addShader(REFRACT_SHADER, "refract.vert", "refract.frag");

		//---------------- setting uniforms

		FurRenderingEngine::setUniform(FUR_SHADER, [](GLuint shader)
		{
			glm::vec3 temp(0.00625, 0.00625, 0.00625); //space in between shells

			int uniformIndex = glGetUniformLocation(shader, "displacement");
			glUniform3fv(uniformIndex, 1, glm::value_ptr(temp));
		}
		);

		glm::vec3 gravity_vec(0.0, -0.5, 0.0);

		FurRenderingEngine::setUniform(FUR_SHADER, [&gravity_vec](GLuint shader)
		{
			int uniformIndex = glGetUniformLocation(shader, "gravity");
			glUniform3fv(uniformIndex, 1, glm::value_ptr(gravity_vec));
		}
		);

		int gravity_effect = grav_effect;

		FurRenderingEngine::setUniform(FUR_SHADER, [&gravity_effect](GLuint shader)
		{
			int uniformIndex = glGetUniformLocation(shader, "gravity_effect");
			glUniform1i(uniformIndex, gravity_effect);
		}
		);

		int cutoff_Layer = cutoffLayer;

		FurRenderingEngine::setUniform(FUR_SHADER, [&cutoff_Layer](GLuint shader)
		{
			int uniformIndex = glGetUniformLocation(shader, "cutoffLayer");
			glUniform1i(uniformIndex, cutoff_Layer);
		}
		);

		FurRenderingEngine::setUniform(FUR_SHADER, [](GLuint shader)
		{
			GLuint colour_texture = FurRenderingEngine::loadBitmap("rainbow_fur.bmp");

			int uniformIndex = glGetUniformLocation(shader, "textureUnit0");
			glUniform1i(uniformIndex, 0);

			uniformIndex = glGetUniformLocation(shader, "textureUnit1");
			glUniform1i(uniformIndex, 1);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, colour_texture);
			glActiveTexture(GL_TEXTURE0);
		}
		);

		//---------------- setting normal mapping uniforms
		FurRenderingEngine::setUniform(NORMAL_SHADER, [](GLuint shader) 
		{
			//Loading normal map texture
			GLuint normal_texture = FurRenderingEngine::loadBitmap("metal-normalmap.bmp");

			int uniformIndex = glGetUniformLocation(shader, "texMap");
			glUniform1i(uniformIndex, 2);

			uniformIndex = glGetUniformLocation(shader, "normalMap");
			glUniform1i(uniformIndex, 3);

			uniformIndex = glGetUniformLocation(shader, "attConst");
			glUniform1f(uniformIndex, attConstant);
			uniformIndex = glGetUniformLocation(shader, "attLinear");
			glUniform1f(uniformIndex, attLinear);
			uniformIndex = glGetUniformLocation(shader, "attQuadratic");
			glUniform1f(uniformIndex, attQuadratic);

			glActiveTexture(GL_TEXTURE3); //Normal map binding
			glBindTexture(GL_TEXTURE_2D, normal_texture);
			glActiveTexture(GL_TEXTURE2); //Texture binding

		}
		);

		//------------------ adding models with initialised shader

		FurRenderingEngine::addModel("fox.obj", glm::vec3(0.0f, 1.0f, -3.0f),
			glm::vec3(0.5f, 0.5f, 0.5f), FUR_OBJ, FUR_SHADER, false, "", 60);

		FurRenderingEngine::addModel("cube.obj", glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(50.0f, 0.1f, 50.0f), PLANE_OBJ, PLANE_SHADER, false, "fabric.bmp", 1);

		//------------------ adding reflection and refraction models with initialised shaders
		FurRenderingEngine::addModel("cube.obj", glm::vec3(2.0f, 1.0f, 0.0f),
			glm::vec3(0.5f, 0.5f, 0.5f), REFLECT_OBJ, REFLECT_SHADER, false, "metal-texturemap.bmp", 1);

		FurRenderingEngine::addModel("cube.obj", glm::vec3(-2.0f, 1.0f, 0.0f),
			glm::vec3(0.5f, 0.5f, 0.5f), REFRACT_OBJ, REFRACT_SHADER, false, "metal-texturemap.bmp", 1);

		//------------------- adding collectables

		std::string collectableId = "collectable";

		for (int i = 0; i < 5; i++)
		{
			collectableId.append(std::to_string(i));

			float randX = rand() % 10 + 1;
			float randZ = rand() % 10 + 1;

			FurRenderingEngine::addModel("cube.obj", glm::vec3(randX, 1.0f, randZ),
				glm::vec3(0.2f, 0.2f, 0.2f), collectableId, PLANE_SHADER, false, "Checkerboard.bmp", 1);

			collectableId = "collectable";
		}

		//------------------- adding normal mapped cube

		FurRenderingEngine::addModel("cube.obj", glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.5f, 0.5f, 0.5f), NORMAL_OBJ, NORMAL_SHADER, true, "metal-texturemap.bmp", 1);

		//------------------- adding skybox

		const char *cubeTexFiles[6] = {
			"town-skybox/Town_bk.bmp", "town-skybox/Town_ft.bmp",
			"town-skybox/Town_rt.bmp", "town-skybox/Town_lf.bmp",
			"town-skybox/Town2_up.bmp", "town-skybox/Town_dn.bmp"
		};

		FurRenderingEngine::addSkybox(cubeTexFiles, "cubeMap.vert", "cubeMap.frag");
	}
}

void update(SDL_Event sdlEvent)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (!TUTORIAL_BASE)
	{
		//-------- rotating around each axis
		if (keys[SDL_SCANCODE_COMMA])
		{
			FurRenderingEngine::updateModelRot(FUR_OBJ, 0.0f, -1.5f, 0.0f);
		}
		if (keys[SDL_SCANCODE_PERIOD])
		{
			FurRenderingEngine::updateModelRot(FUR_OBJ, 0.0f, 1.5f, 0.0f);
		}

		//-------------- movement
		if (keys[SDL_SCANCODE_W])
		{
			FurRenderingEngine::updateModelPos(FUR_OBJ, 0.0f, 0.0f, -0.1f);
		}
		if (keys[SDL_SCANCODE_S])
		{
			FurRenderingEngine::updateModelPos(FUR_OBJ, 0.0f, 0.0f, 0.1f);
		}
		if (keys[SDL_SCANCODE_A])
		{
			FurRenderingEngine::updateModelPos(FUR_OBJ, -0.1f, 0.0f, 0.0f);
		}
		if (keys[SDL_SCANCODE_D])
		{
			FurRenderingEngine::updateModelPos(FUR_OBJ, 0.1f, 0.0f, 0.0f);
		}

		//----------- camera controls
		if (keys[SDL_SCANCODE_Z])
		{
			FurRenderingEngine::zoomToModel(FUR_OBJ, 0.1f);
			std::cout << "zoom out on " << FUR_OBJ << std::endl;
		}
		if (keys[SDL_SCANCODE_X])
		{
			FurRenderingEngine::zoomToModel(FUR_OBJ, -0.1f);
			std::cout << "zoom in on " << FUR_OBJ << std::endl;
		}

		//------------ regenerating fur texture 
		//	- could be used in conjunction with setFurChance to adjust how much fur an object has
		if (keys[SDL_SCANCODE_R])
		{
			FurRenderingEngine::regenTexture(FUR_OBJ);
			std::cout << "regenerate texture on " << FUR_OBJ << std::endl;
		}

		if (keys[SDL_SCANCODE_F])
		{
			furAmount++;
			if (furAmount > 100)
				furAmount = 100;

			FurRenderingEngine::setFurChance(furAmount);
			FurRenderingEngine::regenTexture(FUR_OBJ);
			std::cout << "increasing fur amount: " << furAmount << std::endl;
		}

		if (keys[SDL_SCANCODE_G])
		{
			furAmount--;
			if (furAmount < 0)
				furAmount = 0;

			FurRenderingEngine::setFurChance(furAmount);
			FurRenderingEngine::regenTexture(FUR_OBJ);
			std::cout << "decreasing fur amount: " << furAmount << std::endl;
		}

		//------------ resetting model rotation
		if (keys[SDL_SCANCODE_Q])
		{
			FurRenderingEngine::resetModelRot(FUR_OBJ);
			std::cout << "reset " << FUR_OBJ << std::endl;
		}

		//----------- changing texture of fox

		if (keys[SDL_SCANCODE_P])
		{
			FurRenderingEngine::setUniform(FUR_SHADER, [](GLuint shader)
			{
				GLuint colour_texture = FurRenderingEngine::loadBitmap("pink_checkerboard.bmp");

				int uniformIndex = glGetUniformLocation(shader, "textureUnit0");
				glUniform1i(uniformIndex, 0);

				uniformIndex = glGetUniformLocation(shader, "textureUnit1");
				glUniform1i(uniformIndex, 1);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, colour_texture);
				glActiveTexture(GL_TEXTURE0);
			}
			);
			std::cout << "changed colour texture of " << FUR_OBJ << " to pink_checkerboard.bmp\n";
		}

		if (keys[SDL_SCANCODE_O])
		{
			FurRenderingEngine::setUniform(FUR_SHADER, [](GLuint shader)
			{
				GLuint colour_texture = FurRenderingEngine::loadBitmap("rainbow_fur.bmp");

				int uniformIndex = glGetUniformLocation(shader, "textureUnit0");
				glUniform1i(uniformIndex, 0);

				uniformIndex = glGetUniformLocation(shader, "textureUnit1");
				glUniform1i(uniformIndex, 1);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, colour_texture);
				glActiveTexture(GL_TEXTURE0);
			}
			);

			std::cout << "changed colour texture of " << FUR_OBJ << " to rainbow_fur.bmp\n";
		}

		//----------- activating and deactivating normal mapping
		if (keys[SDL_SCANCODE_8])
		{
			FurRenderingEngine::setShader(NORMAL_OBJ, PLANE_SHADER);
			std::cout << "set shader of " << NORMAL_OBJ << " to " << PLANE_SHADER << std::endl;
		}
		if (keys[SDL_SCANCODE_9])
		{
			FurRenderingEngine::setShader(NORMAL_OBJ, NORMAL_SHADER);
			std::cout << "set shader of " << NORMAL_OBJ << " to " << NORMAL_SHADER << std::endl;
		}

		//----------- activating and deactivating reflection 
		if (keys[SDL_SCANCODE_K])
		{
			FurRenderingEngine::setShader(REFLECT_OBJ, PLANE_SHADER);
			std::cout << "set shader of " << REFLECT_OBJ << " to " << PLANE_SHADER << std::endl;
		}

		if (keys[SDL_SCANCODE_L])
		{
			FurRenderingEngine::setShader(REFLECT_OBJ, REFLECT_SHADER);
			std::cout << "set shader of " << REFLECT_OBJ << " to " << REFLECT_SHADER << std::endl;
		}

		//----------- activating and deactivating refraction 
		if (keys[SDL_SCANCODE_N])
		{
			FurRenderingEngine::setShader(REFRACT_OBJ, PLANE_SHADER);
			std::cout << "set shader of " << REFRACT_OBJ << " to " << PLANE_SHADER << std::endl;
		}

		if (keys[SDL_SCANCODE_M])
		{
			FurRenderingEngine::setShader(REFRACT_OBJ, REFRACT_SHADER);
			std::cout << "set shader of " << REFRACT_OBJ << " to " << REFRACT_SHADER << std::endl;
		}


		//----------- increasing and decreasing gravity
		// - modifying the gravity vector does not make much difference
		if (keys[SDL_SCANCODE_1])
		{
			int gravity_effect = --grav_effect;
			if (gravity_effect < 0 || grav_effect < 0)
			{
				gravity_effect = 0;
				grav_effect = 0;
			}
			std::cout << "increasing gravity: " << grav_effect << std::endl;

			FurRenderingEngine::setUniform(FUR_SHADER, [&gravity_effect](GLuint shader)
			{
				int uniformIndex = glGetUniformLocation(shader, "gravity_effect");
				glUniform1i(uniformIndex, gravity_effect);
			}
			);
		}
		if (keys[SDL_SCANCODE_2])
		{
			int gravity_effect = ++grav_effect;
			if (gravity_effect > 100 || grav_effect > 100)
			{
				gravity_effect = 100;
				grav_effect = 100;
			}
			std::cout << "decreasing gravity: " << grav_effect << std::endl;

			FurRenderingEngine::setUniform(FUR_SHADER, [&gravity_effect](GLuint shader)
			{
				int uniformIndex = glGetUniformLocation(shader, "gravity_effect");
				glUniform1i(uniformIndex, gravity_effect);
			}
			);
		}

		//----------- increasing and decreasing fur length
		// - cutoffLayer is where the fur will end if it is less than or equal to number of layers
		if (keys[SDL_SCANCODE_3])
		{
			int cutoff_Layer = --cutoffLayer;
			if (cutoffLayer < 1 || cutoff_Layer < 1)
			{
				cutoffLayer = 1;
				cutoff_Layer = 1;
			}
			std::cout << "decreasing maximum no of layers: " << cutoffLayer << std::endl;

			FurRenderingEngine::setUniform(FUR_SHADER, [&cutoff_Layer](GLuint shader)
			{
				int uniformIndex = glGetUniformLocation(shader, "cutoffLayer");
				glUniform1i(uniformIndex, cutoff_Layer);
			}
			);
		}
		if (keys[SDL_SCANCODE_4])
		{
			int cutoff_Layer = ++cutoffLayer;
			if (cutoffLayer > 200 || cutoff_Layer > 200)
			{
				cutoffLayer = 200;
				cutoff_Layer = 200;
			}
			std::cout << "increasing maximum no of layers: " << cutoffLayer << std::endl;

			FurRenderingEngine::setUniform(FUR_SHADER, [&cutoff_Layer](GLuint shader)
			{
				int uniformIndex = glGetUniformLocation(shader, "cutoffLayer");
				glUniform1i(uniformIndex, cutoff_Layer);
			}
			);
		}

		//----------- layers is how many times the model will be rendered, regardless of what the cutoff layer is
		if (keys[SDL_SCANCODE_5])
		{
			int num_layers = --layers;
			if (num_layers < 1 || layers < 1)
			{
				num_layers = 1;
				layers = 1;
			}

			std::cout << "decreasing layers: " << layers << std::endl;

			FurRenderingEngine::setNumLayers(FUR_OBJ, num_layers);
		}
		if (keys[SDL_SCANCODE_6])
		{
			int num_layers = ++layers;
			if (num_layers > 200 || layers > 200)
			{
				num_layers = 200;
				layers = 200;
			}
			std::cout << "increasing layers: " << layers << std::endl;

			FurRenderingEngine::setNumLayers(FUR_OBJ, num_layers);
		}
	}
}

void draw()
{
	FurRenderingEngine::draw();
	//other draw code could be added here
}

int main(int argc, char *argv[])
{
	SDL_Window * window;
	SDL_GLContext glContext;

	window = setupRC(glContext);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(1);
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	init();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {

			//when closing the window or pressing the escape key, close the program
			if (sdlEvent.type == SDL_QUIT || sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				running = false;

		}

		update(sdlEvent);

		draw();
		SDL_GL_SwapWindow(window); // swap buffers
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
