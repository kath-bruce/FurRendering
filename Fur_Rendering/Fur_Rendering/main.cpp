#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "FurRenderingEngine.h"
#include <SDL.h>

#define FUR_SHADER "fur_shader"
#define FUR_OBJ "fur_Obj"

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

void setDisplacement(GLuint shader)
{
	glm::vec3 temp(0.00625, 0.00625, 0.00625); //space in between shells

	int uniformIndex = glGetUniformLocation(shader, "displacement");
	glUniform3fv(uniformIndex, 1, glm::value_ptr(temp));
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*
	//shaders
	FurRenderingEngine::addShader(FUR_SHADER, "fur.vert", "fur.frag");

	FurRenderingEngine::setUniform(FUR_SHADER, [](GLuint shader)
	{
		glm::vec3 temp(0.00625, 0.00625, 0.00625); //space in between shells

		int uniformIndex = glGetUniformLocation(shader, "displacement");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(temp));
	}
	);

	glm::vec3 gravity(0.0, -0.5, 0.0);

	FurRenderingEngine::setUniform(FUR_SHADER, [&gravity](GLuint shader)
	{
		int uniformIndex = glGetUniformLocation(shader, "gravity");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(gravity));
	}
	);

	FurRenderingEngine::setUniform(FUR_SHADER, [](GLuint shader)
	{
		glm::vec4 temp(1.0, 0.6, 0.0, 0.0);

		int uniformIndex = glGetUniformLocation(shader, "fur_colour");
		glUniform4fv(uniformIndex, 1, glm::value_ptr(temp));
	}
	);
	
	int num_layers = 60;

	FurRenderingEngine::setNumLayers(num_layers);

	int gravity_effect = 60;

	FurRenderingEngine::setUniform(FUR_SHADER, [&gravity_effect](GLuint shader)
	{
		int uniformIndex = glGetUniformLocation(shader, "gravity_effect");
		glUniform1i(uniformIndex, gravity_effect);

		FurRenderingEngine::setGravityEffect(gravity_effect);
	}
	);

	int fur_chance = 30;

	FurRenderingEngine::setFurChance(fur_chance);

	FurRenderingEngine::addModel("fox.obj", glm::vec3(0.0f, 1.0f, -2.1f),
		glm::vec3(0.5f, 0.5f, 0.5f), FUR_OBJ, FUR_SHADER);*/

	//enter init code here

	FurRenderingEngine::addShader("fur", "fur_tut.vert", "fur_tut.frag");

	FurRenderingEngine::addModel("cube.obj", glm::vec3(0.0f, 1.0f, -2.0f), glm::vec3(0.5f, 0.5f, 0.5f), "cube", "fur");

	FurRenderingEngine::setNumLayers(60);

	FurRenderingEngine::setUniform("fur", setDisplacement);

	FurRenderingEngine::setUniform("fur", [](GLuint shader)
	{
		glm::vec4 temp(1.0, 0.6, 0.0, 0.0);

		int uniformIndex = glGetUniformLocation(shader, "fur_colour");
		glUniform4fv(uniformIndex, 1, glm::value_ptr(temp));
	}
	);

	glm::vec3 gravity(0.0, -0.5, 0.0);

	FurRenderingEngine::setUniform("fur", [&gravity](GLuint shader)
	{
		int uniformIndex = glGetUniformLocation(shader, "gravity");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(gravity));
	}
	);

	int gravity_effect = 60;

	FurRenderingEngine::setUniform("fur", [&gravity_effect](GLuint shader)
	{
		int uniformIndex = glGetUniformLocation(shader, "gravity_effect");
		glUniform1i(uniformIndex, gravity_effect);

		FurRenderingEngine::setGravityEffect(gravity_effect);
	}
	);
}

void update(SDL_Event sdlEvent)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_COMMA])
	{
		FurRenderingEngine::updateModelRot("cube", 0.0f, 1.5f, 0.0f);
	}
	if (keys[SDL_SCANCODE_PERIOD])
	{
		FurRenderingEngine::updateModelRot("cube", 0.0f, -1.5f, 0.0f);
	}
	/*if (keys[SDL_SCANCODE_UP])
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
	if (keys[SDL_SCANCODE_R])
	{
		FurRenderingEngine::regenTexture(FUR_OBJ);
	}
	if (keys[SDL_SCANCODE_Q])
	{
		FurRenderingEngine::resetModelRot(FUR_OBJ);
	}*/

	//enter keyboard handling and pther update code here
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
			if (sdlEvent.type == SDL_QUIT || sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				running = false;

		}

		update(sdlEvent);

		draw(); //can be called in custom draw method
		SDL_GL_SwapWindow(window); // swap buffers
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
