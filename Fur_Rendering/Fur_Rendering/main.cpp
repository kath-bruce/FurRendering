#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "FurRenderingEngine.h"
#include <SDL.h>

#define FUR_SHADER "fur_shader"
#define FUR_OBJ "fur_Obj"

//the magnitude of the y value describes how intense the gravity is
int grav_effect = 60;

int layers = 60;
int cutoffLayer = 20;

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

	
	//shaders
	FurRenderingEngine::addShader(FUR_SHADER, "fur.vert", "fur.frag");

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

	FurRenderingEngine::setUniform(FUR_SHADER, [](GLuint shader)
	{
		glm::vec4 temp(1.0, 0.6, 0.0, 0.0);

		int uniformIndex = glGetUniformLocation(shader, "fur_colour");
		glUniform4fv(uniformIndex, 1, glm::value_ptr(temp));
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
	
	//---------------- setting fur rendering engine values

	int num_layers = layers;

	FurRenderingEngine::setNumLayers(num_layers);

	//------------------ adding models with initialised shader

	FurRenderingEngine::addModel("fox.obj", glm::vec3(0.0f, 1.0f, -2.1f),
		glm::vec3(0.5f, 0.5f, 0.5f), FUR_OBJ, FUR_SHADER);

	
}

void update(SDL_Event sdlEvent)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	//-------- rotating around each axis
	if (keys[SDL_SCANCODE_COMMA])
	{
		FurRenderingEngine::updateModelRot(FUR_OBJ, 0.0f, 1.5f, 0.0f);
	}
	if (keys[SDL_SCANCODE_PERIOD])
	{
		FurRenderingEngine::updateModelRot(FUR_OBJ, 0.0f, -1.5f, 0.0f);
	}
	if (keys[SDL_SCANCODE_UP])
	{
		FurRenderingEngine::updateModelRot(FUR_OBJ, 1.5f, 0.0f, 0.0f);
	}
	if (keys[SDL_SCANCODE_DOWN])
	{
		FurRenderingEngine::updateModelRot(FUR_OBJ, -1.5f, 0.0f, 0.0f);
	}
	if (keys[SDL_SCANCODE_LEFT])
	{
		FurRenderingEngine::updateModelRot(FUR_OBJ, 0.0f, 0.0f, 1.5f);
	}
	if (keys[SDL_SCANCODE_RIGHT])
	{
		FurRenderingEngine::updateModelRot(FUR_OBJ, 0.0f, 0.0f, -1.5f);
	}

	//------------ regenerating fur texture 
	//	- could be used in conjunction with setFurChance to adjust how much fur an object has
	if (keys[SDL_SCANCODE_R])
	{
		FurRenderingEngine::regenTexture(FUR_OBJ);
	}

	//------------ resetting model rotation
	if (keys[SDL_SCANCODE_Q])
	{
		FurRenderingEngine::resetModelRot(FUR_OBJ);
	}

	//----------- increasing and decreasing gravity
	if (keys[SDL_SCANCODE_1])
	{
		int gravity_effect = --grav_effect;

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

		FurRenderingEngine::setUniform(FUR_SHADER, [&gravity_effect](GLuint shader)
		{
			int uniformIndex = glGetUniformLocation(shader, "gravity_effect");
			glUniform1i(uniformIndex, gravity_effect);
		}
		);
	}

	//----------- increasing and decreasing fur length
	if (keys[SDL_SCANCODE_3])
	{
		int cutoff_Layer = --cutoffLayer;

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

		FurRenderingEngine::setUniform(FUR_SHADER, [&cutoff_Layer](GLuint shader)
		{
			int uniformIndex = glGetUniformLocation(shader, "cutoffLayer");
			glUniform1i(uniformIndex, cutoff_Layer);
		}
		);
	}
	if (keys[SDL_SCANCODE_5])
	{
		int num_layers = --layers;

		FurRenderingEngine::setNumLayers(num_layers);
	}
	if (keys[SDL_SCANCODE_6])
	{
		int num_layers = ++layers;

		FurRenderingEngine::setNumLayers(num_layers);
	}

}

void draw()
{
	FurRenderingEngine::draw();
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
