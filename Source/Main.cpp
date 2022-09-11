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
#include "Camera.h"
#include "Arwing.h"
#include <reactphysics3d/reactphysics3d.h>

// Global variables
float constexpr gWorldRadius = 2.e5f;
bool	gKeyboardStates[GLFW_KEY_LAST+1];	// Keys states
bool	gMouseRightPressed;					// Mouse button state
bool	gMouseMiddlePressed;				// Mouse button state
bool	gMouseLeftPressed;					// Mouse button state
float	gMousePositionX;					// Mouse position (relative to the start position)
float	gMousePositionY;					// Mouse position (relative to the start position)
float	gMouseRelativeX;					// Mouse relative movement
float	gMouseRelativeY;					// Mouse relative movement

rp3d::PhysicsCommon gPhysics;
rp3d::PhysicsWorld* gWorld;

//Physics		gPhysics;
CModel		gModelSky;
glm::mat4	gMat4Sky;

CModel			gModelCube;
glm::mat4		gMat4Cube1(1.f);
rp3d::RigidBody* gCube1RigidBody = nullptr;
glm::mat4		gMat4Cube2(1.f);
rp3d::RigidBody* gCube2RigidBody = nullptr;

CModel			gModelAsteroid;
glm::mat4		gMat4Asteroid(1.f);
btRigidBody*	gCollisionAsteroid = NULL;

glm::mat4		gProj = glm::perspective(45.f, 1.3f, 1.f, 350000.f);

CArwing gArwing;
CCamera<30> gCamera(gArwing.GetCameraTarget(), 0.f, 1.f);

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

void InitializeOpenGLView(GLFWwindow* pWindow)
{
	
}

// dt in seconds
void UpdateGameLogic(float dt)
{
	if (gKeyboardStates[GLFW_KEY_UP]) gArwing.GoUp(dt);
	if (gKeyboardStates[GLFW_KEY_DOWN]) gArwing.GoDown(dt);
	if (gKeyboardStates[GLFW_KEY_RIGHT]) gArwing.TurnRight(dt);
	if (gKeyboardStates[GLFW_KEY_LEFT]) gArwing.TurnLeft(dt);
	if (gKeyboardStates[GLFW_KEY_SPACE]) { gArwing.Accelerate(dt); gCamera.ChaseTarget(); }

	gArwing.Move(dt);

	gCamera.UpdateViewMatrix(gArwing.GetCameraTarget(), dt);

	//gCollisionArwing->getMotionState()->setWorldTransform(gPhysics.glmMat4TobtTransform(gMat4Arwing));
	// gPhysics.UpdatePhysics(dt);
	gWorld->update(1.f / 60.f);
	gCube1RigidBody->getTransform().getOpenGLMatrix(reinterpret_cast<rp3d::decimal*>(&gMat4Cube1));
	gCube2RigidBody->getTransform().getOpenGLMatrix(reinterpret_cast<rp3d::decimal*>(&gMat4Cube2));
}

void DrawGame(float truc)
{
	glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// MVP
	glm::mat4 model		= glm::mat4(1.0);
	glm::mat4 view		= gCamera.GetViewMatrix();
	glm::mat4 proj		= gProj;

	glm::vec3 cameraPosition = gCamera.GetPosition();

	// Light sources
	glm::vec3 lightPos	= glm::vec3(0.f, 1000.f, 0.f);
	glm::vec3 lightColor= glm::vec3(1.f, 1.f, 1.f);

	// Skybox
	gModelSky.Draw(cameraPosition, gMat4Sky, view, gProj, lightPos, lightColor);
	
	// Arwing
	gArwing.Draw(cameraPosition, view, gProj, lightPos, lightColor);
	
	// Cube
	gModelCube.Draw(cameraPosition, gMat4Cube1, view, gProj, lightPos, lightColor, true);
	gModelCube.Draw(cameraPosition, gMat4Cube2, view, gProj, lightPos, lightColor, true);
	
	// Asteroid
	gModelAsteroid.Draw(cameraPosition, gMat4Asteroid, view, gProj, lightPos, lightColor);

	// gFont.Draw("plop !",20,20,30.0f);
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
	GLFWwindow* pWindow = glfwCreateWindow(1300, 1000, "Hello World", NULL, NULL);
	if (pWindow == NULL)
	{
		glfwTerminate();
		return NULL;
	}
	InitInputs(pWindow);
	glfwMakeContextCurrent(pWindow);	// Make the window's context current

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

	// ===================================================
	// Load images/3d objects/shaders before the main loop
	// ===================================================

	if (gModelSky.Load(ROOT_DIR"Resources\\Meshes\\SpaceBox\\space.obj") == false)
	{
		ConsoleWriteErr("Failed to load 3D object");
	}
	gArwing.LoadModel();
	if (gModelAsteroid.Load(ROOT_DIR"Resources\\Meshes\\Asteroid\\asteroid.obj") == false)
	{
		ConsoleWriteErr("Failed to load 3D object");
	}
	if (gModelCube.Load(ROOT_DIR"Resources\\Meshes\\Cube\\cube.obj") == false)
	{
		ConsoleWriteErr("Failed to load 3D object");
	}
	// Physics world.
	gWorld = gPhysics.createPhysicsWorld();

	gMat4Sky = glm::mat4(1.f);
	gMat4Sky = glm::scale(gMat4Sky, glm::vec3(2.e5f, 2.e5f, 2.e5f));

	gMat4Cube1 = glm::translate(gMat4Cube1, glm::vec3(0.f, 0.f, -10.f));
	gMat4Cube2 = glm::translate(gMat4Cube2, glm::vec3(0.2f, 10.f, -10.f));
	rp3d::Transform cubeTransform;
	cubeTransform.setFromOpenGL(reinterpret_cast<rp3d::decimal*>(&gMat4Cube1));
	gCube1RigidBody = gWorld->createRigidBody(cubeTransform);
	cubeTransform.setFromOpenGL(reinterpret_cast<rp3d::decimal*>(&gMat4Cube2));
	gCube2RigidBody = gWorld->createRigidBody(cubeTransform);
	const rp3d::Vector3 halfExtents(0.5f, 0.5f, 0.5f);
	rp3d::BoxShape* boxShape = gPhysics.createBoxShape(halfExtents);
	gCube1RigidBody->addCollider(boxShape, rp3d::Transform::identity());
	gCube2RigidBody->addCollider(boxShape, rp3d::Transform::identity());
	gCube1RigidBody->enableGravity(false);

	gMat4Asteroid = glm::translate(gMat4Asteroid, glm::vec3(0.f, 0.f, -7.f));
	gMat4Asteroid = glm::scale(gMat4Asteroid, glm::vec3(0.007f, 0.007f, 0.007f));

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

	// Loop until the user closes the window
	float lastTimeInSecond = (float)glfwGetTime();
	while (!glfwWindowShouldClose(pWindow))
	{
		// Update the game (logic/physic/etc...)
		float currTimeInSecond  = (float)glfwGetTime();
		float deltaTimeInSecond = currTimeInSecond - lastTimeInSecond;		// Duration of the last frame, useful to update the game physic.
		lastTimeInSecond = currTimeInSecond;
		UpdateGameLogic(deltaTimeInSecond);

		// Render here
		DrawGame(fabs(sin(lastTimeInSecond*2)));

		// Swap front and back buffers
		glfwSwapBuffers(pWindow);

		// Poll for and process events -> Appel automatique si besoin des fonctions callback_xxx() définies plus haut.
		glfwPollEvents();
	}

	//gPhysics.Terminate();
	glfwTerminate();
	return 0;
}
