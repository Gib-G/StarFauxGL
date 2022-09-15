#include "Types.h"
#include "FileUtil.h"
#include "CImage.h"
#include "Model.h"
#include "Font.h"
#include "World.h"
#include <reactphysics3d/reactphysics3d.h>

// Default window dimensions in pixels.
int constexpr gDefaultWindowWidth = 1100;
int constexpr gDefaultWindowHeight = 1100;
float gAspectRatio = float(gDefaultWindowWidth / gDefaultWindowHeight);

void callback_error(int Error, const char* Description) { fprintf(stderr, "Error %d : %s\n", Error, Description); }

// Initializes OpenGL with GLFW and GLew.
GLFWwindow* Initialize()
{
	// Initialize GLFW.
	glfwSetErrorCallback(callback_error);

	if (!glfwInit()) return NULL;
	else
	{
		string version = glfwGetVersionString();
		ConsoleWriteOk("GLFW (window/input library) :");
		ConsoleWrite  (" -> version : %s", version.c_str());
	}

	// Setup OpenGL with GLFW (we set OpenGL 3.3 minimum, window not resizable).
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creating the window.
	GLFWwindow* pWindow = glfwCreateWindow(gDefaultWindowWidth, gDefaultWindowHeight, "StarFaux GL", nullptr, nullptr);
	if (pWindow == NULL) { glfwTerminate(); return nullptr; }
	glfwMakeContextCurrent(pWindow);

	// Initializing GLew.
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) { ConsoleWriteOk("GLew error: %s", glewGetErrorString(glewErr)); return nullptr; }
	else
	{
		string version = (const char*)glewGetString(GLEW_VERSION);
		ConsoleWriteOk("GLew (OpenGL extender) :");
		ConsoleWrite  (" -> version : %s", version.c_str());
	}

	// Setting up OpenGL.
	// Viewport.
	int width, height;
	glfwGetFramebufferSize(pWindow, &width, &height);
	glViewport(0, 0, width, height);

	// Enable Z-buffer.
	glEnable(GL_DEPTH_TEST);

	// Disable back face culling.
	glDisable(GL_CULL_FACE);

	return pWindow;
}

int main()
{
	GLFWwindow* const window = Initialize();
	if (!window) return -1;

	CWorld World(window);
	glfwSetWindowUserPointer(window, &World);
	glfwSetKeyCallback
	(
		window,
		[](GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
		{
			((CWorld*)glfwGetWindowUserPointer(Window))->HandleKeyboardInputs(Key, Scancode, Action, Mods);
		}
	);

	// In s.
	float previousTime = float(glfwGetTime());
	// Game loop.
	while (!glfwWindowShouldClose(window))
	{
		// In s.
		float const currentTime  = float(glfwGetTime());
		float const Dt = currentTime - previousTime;
		previousTime = currentTime;
		
		// Update the game world.
		World.Update(Dt);

		// Clear the screen and render the game world.
		glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		World.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}