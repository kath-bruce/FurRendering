#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "FurRenderingEngine.h"
#include <SDL.h>

#define FUR_PLANE "furPlane"

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
	FurRenderingEngine::addShader(FUR_PLANE, "fur.vert", "fur.frag");

	FurRenderingEngine::setUniform(FUR_PLANE, [](GLuint shader)
	{
		glm::vec3 temp(0.0125, 0.0125, 0.0125); //space in between shells

		int uniformIndex = glGetUniformLocation(shader, "displacement");
		glUniform3fv(uniformIndex, 1, glm::value_ptr(temp));
	}
	);


	//models
	/*FurRenderingEngine::addModel("cube.obj", glm::vec3(0.0f, 1.0f, -3.0f), 
		glm::vec3(0.5f, 0.5f, 0.5f), FUR_PLANE, FUR_PLANE);*/
	
	FurRenderingEngine::addModel("fox.obj", glm::vec3(0.0f, 1.0f, -3.0f),
		glm::vec3(0.5f, 0.5f, 0.5f), FUR_PLANE, FUR_PLANE);
	
}

void update(SDL_Event sdlEvent)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_COMMA])
	{
		FurRenderingEngine::updateModelRot(FUR_PLANE, 0.0f, 1.5f, 0.0f);
	}
	if (keys[SDL_SCANCODE_PERIOD])
	{
		FurRenderingEngine::updateModelRot(FUR_PLANE, 0.0f, -1.5f, 0.0f);
	}
	if (keys[SDL_SCANCODE_UP])
	{
		FurRenderingEngine::updateModelRot(FUR_PLANE, 1.5f, 0.0f, 0.0f);
	}
	if (keys[SDL_SCANCODE_DOWN])
	{
		FurRenderingEngine::updateModelRot(FUR_PLANE, -1.5f, 0.0f, 0.0f);
	}
	if (keys[SDL_SCANCODE_LEFT])
	{
		FurRenderingEngine::updateModelRot(FUR_PLANE, 0.0f, 0.0f, 1.5f);
	}
	if (keys[SDL_SCANCODE_RIGHT])
	{
		FurRenderingEngine::updateModelRot(FUR_PLANE, 0.0f, 0.0f, -1.5f);
	}
	if (keys[SDL_SCANCODE_R])
	{
		FurRenderingEngine::regenTexture(FUR_PLANE);
	}

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

		FurRenderingEngine::draw();
		SDL_GL_SwapWindow(window); // swap buffers
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}