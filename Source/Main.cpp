// application.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Run program: Ctrl + F5 or Menu Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
// Add/remove a breakpoint in the code : F9 on the line you want your breakpoint (the program must be built in debug and ran in debug).

// Tips for Getting Started: 
//   Use the Solution Explorer window to add/manage files
//   Use the Output window to see build output and other messages

// Docs :
// GLFW	  : https://www.glfw.org/docs/latest/modules.html						(Library to handle window, input, OpenGL contexts)
// glm	  : https://glm.g-truc.net/0.9.9/index.html								(Library for OpenGL math (vectors, matrices, etc))
// assimp : https://assimp-docs.readthedocs.io/en/latest/usage/use_the_lib.html	(Library to load 3D objects)

// exemple	: https://github.com/zhihui-xie/CG_OpenGL

#include "Types.h"
#include "FileUtil.h"
#include "CImage.h"
#include "Model.h"
#include "Font.h"
#include "Physics.h"
#include "World.h"
#include <reactphysics3d/reactphysics3d.h>
//#include <GLFW/glfw3.h>

// Global variables
bool	gKeyboardStates[GLFW_KEY_LAST+1];	// Keys states
bool	gMouseRightPressed;					// Mouse button state
bool	gMouseMiddlePressed;				// Mouse button state
bool	gMouseLeftPressed;					// Mouse button state
float	gMousePositionX;					// Mouse position (relative to the start position)
float	gMousePositionY;					// Mouse position (relative to the start position)
float	gMouseRelativeX;					// Mouse relative movement
float	gMouseRelativeY;					// Mouse relative movement

CModel gCube;
glm::mat4 gMat4Cube(1.f);

// Default window dimensions in pixels.
int constexpr gDefaultWindowWidth = 1100;
int constexpr gDefaultWindowHeight = 1100;
float gAspectRatio = float(gDefaultWindowWidth / gDefaultWindowHeight);

void callback_error(int error, const char* description)
{
	fprintf(stderr, "Error %d : %s\n", error, description);
}

void callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	if (action == GLFW_PRESS)
	{
		gKeyboardStates[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		gKeyboardStates[key] = false;
	}
}

void callback_mouse(GLFWwindow * window, double xpos, double ypos)
{
	static bool  bFirstTime     = true;
	static float mouseStartPosX = 0.0;
	static float mouseStartPosY = 0.0;
	static float mouseLastPosX  = 0.0;
	static float mouseLastPosY  = 0.0;

	if (bFirstTime)
	{
		mouseStartPosX = 0.0f;
		mouseStartPosY = 0.0f;
		mouseLastPosX  =  (float)xpos;
		mouseLastPosY  = -(float)ypos;
		bFirstTime = false;
	}

	gMousePositionX =  ((float)xpos - mouseStartPosX);
	gMousePositionY = -((float)ypos - mouseStartPosY);
	gMouseRelativeX = gMousePositionX - mouseLastPosX;
	gMouseRelativeY = gMousePositionY - mouseLastPosY;
	mouseLastPosX   = gMousePositionX;
	mouseLastPosY   = gMousePositionY;

//	ConsoleWrite("%3.2f, %3.2f",gMouseRelativeX,gMouseRelativeY);
//	ConsoleWrite("%3.2f, %3.2f - %3.2f, %3.2f",gMousePositionX,gMousePositionY,xpos,ypos);
}

void callback_mouseButton(GLFWwindow * window, int button, int action, int mods)
{
	bool bIsButtonPressed = (action == GLFW_PRESS);
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		gMouseRightPressed = bIsButtonPressed;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		gMouseMiddlePressed = bIsButtonPressed;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		gMouseLeftPressed = bIsButtonPressed;
	}
}

void callback_mouseWheel(GLFWwindow* window, double xoffset, double yoffset)
{
	// TODO if needed
}

void InitInputs(GLFWwindow* pWindow)
{
	// Connect callback for input (keyboard, mouse), hide the mouse cursor.
	glfwSetInputMode			(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback			(pWindow, callback_keyboard);
	glfwSetCursorPosCallback	(pWindow, callback_mouse);
	glfwSetMouseButtonCallback	(pWindow, callback_mouseButton);
	glfwSetScrollCallback		(pWindow, callback_mouseWheel);

	for (size_t i=0; i<sizeofarray(gKeyboardStates); i++)
	{
		gKeyboardStates[i] = false;
	}
	gMouseRightPressed	= false;
	gMouseMiddlePressed	= false;
	gMouseLeftPressed	= false;
	gMouseRelativeX		= 0.0f;
	gMouseRelativeY		= 0.0f;
}

GLFWwindow* InitializeEverything()
{
	// Initialize GLFW
	glfwSetErrorCallback(callback_error);
	if (!glfwInit())
	{
		return NULL;
	}
	else
	{
		string version = glfwGetVersionString();
		ConsoleWriteOk("GLFW (window/input library) :");
		ConsoleWrite  (" -> version : %s", version.c_str());
	}

	// Setup OpenGL with GLFW (we set OpenGL 3.3 minimum, window not resizable)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a windowed mode window and its OpenGL context
	//GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
	//GLFWvidmode const* const videoMode = glfwGetVideoMode(monitor);
	//int const monitorWidth = videoMode->width;
	//int const monitorHeight = videoMode->height;

	GLFWwindow* pWindow = glfwCreateWindow(gDefaultWindowWidth, gDefaultWindowHeight, "StarFaux GL", nullptr, nullptr);
	if (pWindow == NULL)
	{
		glfwTerminate();
		return NULL;
	}
	//gAspectRatio = float(monitorWidth / monitorHeight);

	InitInputs(pWindow);
	glfwMakeContextCurrent(pWindow);

	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
	{
		ConsoleWriteOk("GLew error : %s", glewGetErrorString(glewErr));
		return NULL;
	}
	else
	{
		string version = (const char*)glewGetString(GLEW_VERSION);
		ConsoleWriteOk("GLew (OpenGL extender) :");
		ConsoleWrite  (" -> version : %s", version.c_str());
	}

	// Setup Bullet
	//gPhysics.Initialize();

	return pWindow;
}

int main()
{
	// ============================================================
	// Initialize libraries, create a window with an OpenGL context
	// ============================================================
	GLFWwindow* pWindow = InitializeEverything();
	if (pWindow == NULL)
	{
		return -1;
	}
	//	if (gFont.Load("images/ascii-font.png") == false)
	//	{
	//		ConsoleWriteErr("Failed to load font image");
	//	}

	// ===================================================
	// Setup de la vue OpenGL
	// ===================================================
	// Set OpenGL viewport
	int width, height;
	glfwGetFramebufferSize(pWindow, &width, &height);
	glViewport(0, 0, width, height);

	// Enable Z-buffer
	glEnable(GL_DEPTH_TEST);

	// Disable back face culling
	glDisable(GL_CULL_FACE);

	CWorld World(pWindow);
	struct SData { CWorld* World = nullptr; float Dt = 0.f; } data{&World, 0.f};
	glfwSetWindowUserPointer(pWindow, &data);
	glfwSetKeyCallback
	(
		pWindow,
		[](GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
		{
			SData const data = *(SData*)glfwGetWindowUserPointer(Window);
			data.World->HandleKeyboardInputs(Key, Scancode, Action, Mods, data.Dt);
		}
	);

	gCube.Load(ROOT_DIR"Resources\\Meshes\\Cube\\cube.obj");
	gMat4Cube = glm::scale(gMat4Cube, glm::vec3(2.e5f, 2.e5f, 2.e5f));

	// In s.
	float previousTime = float(glfwGetTime());
	// Game loop.
	while (!glfwWindowShouldClose(pWindow))
	{
		// In s.
		float const currentTime  = float(glfwGetTime());
		float const Dt = currentTime - previousTime;
		data.Dt = Dt;
		previousTime = currentTime;
		
		// Update the game world.
		World.Update(Dt);

		// Clear the screen and render the game world.
		glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		World.Render();

		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	//gPhysics.Terminate();
	glfwTerminate();
	return 0;
}
