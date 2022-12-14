#include "World.h"

CWorld::CWorld(GLFWwindow* const Window) : Window(Window)
{
	// Creating a standard perpective projection matrix.
	assert(Window);
	int windowWidth, windowHeight;
	glfwGetFramebufferSize(Window, &windowWidth, &windowHeight);
	ProjectionMatrix = glm::perspective(45.f, float(windowWidth) / float(windowHeight), 1.f, 350000.f);

	// ReactPhysics3D stuff.
	PhysicsWorld = PhysicsCommon.createPhysicsWorld();
	PhysicsWorld->setIsGravityEnabled(false);
	PhysicsWorld->setEventListener(&CollisionListener);

	// Loading models of the game.
	ArwingModel.Load(ROOT_DIR"Resources\\Meshes\\Arwing\\arwing_starlink.fbx");
	AsteroidModel.Load(ROOT_DIR"Resources\\Meshes\\Cube\\Cube.obj");
	// AsteroidModel.Load(ROOT_DIR"Resources\\Meshes\\Asteroid\\asteroid.obj"); // Too many triangles, ?a met mon GPU en PLS !
	LaserModel.Load(ROOT_DIR"Resources\\Meshes\\Cube\\Cube.obj");
	SpaceBoxModel.Load(ROOT_DIR"Resources\\Meshes\\SpaceBox\\space.obj");
	SpaceBoxModelMatrix = glm::scale(SpaceBoxModelMatrix, glm::vec3(WorldHalfExtent));

	// Setting up the Arwing (the spacecraft controlled by the player).
	Arwing.SetModel(&ArwingModel);
	Arwing.InitializeRigidBody(PhysicsCommon, PhysicsWorld);
	
	// Filling the asteroid pool for constant-time acces and no instatiations in-game.
	CAsteroid asteroid(this, &AsteroidModel);
	asteroid.InitializeRigidBody(PhysicsCommon, PhysicsWorld);
	AsteroidPool.FillWith(asteroid);
	// asteroid is deleted at the end of this function.
	// We call DestroyRigidBody to avoid that its rigid body remains dangling somewhere in ether...
	asteroid.DestroyRigidBody(PhysicsWorld);

	// Boom! Spawn 100 asteroids in one go!
	for (int k = 0; k < 100; k++) SpawnAsteroid();
}

void CWorld::Update(float const Dt)
{
	// Arwing regular update.
	Arwing.Update(Dt);
	Camera.UpdateViewMatrix(Arwing.GetCameraTarget()); // ? mettre plus bas peut-?tre...

	// Physics update.
	TimeAccumulator += Dt;
	while (TimeAccumulator >= PhysicsDt)
	{
		PhysicsWorld->update(PhysicsDt);
		TimeAccumulator -= PhysicsDt;
	}
	InterpolationFactor = TimeAccumulator / PhysicsDt;
	assert(0.f <= InterpolationFactor && InterpolationFactor <= 1.f);

	// Asteroids regular updates. There should have been the same thing for laser projectiles...
	AsteroidPool.UpdateAllActiveEntities(Dt);

	_Time += Dt;
	if (_Time >= AsteroidSpawnTime)
	{
		for (uint16_t k = 0; k < AsteroidsToSpawn; k++) SpawnAsteroid();
		_Time = 0.f;
	}
}

void CWorld::Render()
{
	glm::vec3 const& cameraPosition = Camera.GetPosition();
	glm::mat4 const& viewMatrix = Camera.GetViewMatrix();

	SpaceBoxModel.Draw(cameraPosition, SpaceBoxModelMatrix, viewMatrix, ProjectionMatrix, LightPosition, LightColor);
	Arwing.Draw(cameraPosition, viewMatrix, ProjectionMatrix, LightPosition, LightColor);

	AsteroidPool.DrawAllActiveEntities(cameraPosition, viewMatrix, ProjectionMatrix, LightPosition, LightColor);
}

void CWorld::HandleKeyboardInputs(int Key, int Scancode, int Action, int Mods)
{
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS) { glfwSetWindowShouldClose(Window, GLFW_TRUE); return; }

	if (Action == GLFW_PRESS)
	{
		if (Key == GLFW_KEY_W) Arwing.ShouldAccelerate = true;
		if (Key == GLFW_KEY_S) Arwing.ShouldDecelerate = true;
		if (Key == GLFW_KEY_UP) Arwing.ShouldGoUp = true;
		if (Key == GLFW_KEY_DOWN) Arwing.ShouldGoDown = true;
		if (Key == GLFW_KEY_RIGHT) Arwing.ShouldTurnRight = true;
		if (Key == GLFW_KEY_LEFT) Arwing.ShouldTurnLeft = true;
	}
	else if (Action == GLFW_RELEASE)
	{
		if (Key == GLFW_KEY_W) Arwing.ShouldAccelerate = false;
		if (Key == GLFW_KEY_S) Arwing.ShouldDecelerate = false;
		if (Key == GLFW_KEY_UP) Arwing.ShouldGoUp = false;
		if (Key == GLFW_KEY_DOWN) Arwing.ShouldGoDown = false;
		if (Key == GLFW_KEY_RIGHT) Arwing.ShouldTurnRight = false;
		if (Key == GLFW_KEY_LEFT) Arwing.ShouldTurnLeft = false;
	}
}

void CWorld::SpawnAsteroid()
{
	CAsteroid* const asteroid = AsteroidPool.GetInactiveEntity();
	if (!asteroid) return;

	CAsteroid::SParams params;
	params.PlayerPosition = Arwing.GetPosition();

	asteroid->Randomize(params);
	asteroid->SetActive(true);
}

void CWorld::InitializeRigidBody(CEntity& Entity)
{
	Entity.InitializeRigidBody(PhysicsCommon, PhysicsWorld);
}