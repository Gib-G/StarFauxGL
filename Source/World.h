#pragma once
#include "CollisionListener.h"
#include "Model.h"
#include "Arwing.h"
#include "Camera.h"
#include "EntityPool.h"

// Basically a container for everything in the game.
class CWorld
{
public:
	CWorld(GLFWwindow* const Window);

	// Dt = dynamic game delta time.
	void Update(float const Dt);
	void Render();

	void HandleKeyboardInputs(int Key, int Scancode, int Action, int Mods);

	void SpawnAsteroid();

	float GetInterpolationFactor() const { return InterpolationFactor; }

	void InitializeRigidBody(CEntity& Entity);

	glm::vec3 GetArwingPosition() const { return Arwing.GetPosition(); }

private:
	// Used to scale the skybox (SpaceBox, 1 * 1 * 1 m cube).
	float const WorldHalfExtent = 2.e5f; // The world is a 400000m cube.

	// Time between two asteroid spawns.
	float const AsteroidSpawnTime = 0.1f; // In s.
	// How many asteroids to spawn at once.
	uint16_t const AsteroidsToSpawn = 2;
	
	// The Arwing, the spacecraft controlled by the player.
	CArwing Arwing = CArwing(this);

	// 3D models.
	CModel ArwingModel;
	CModel AsteroidModel;
	CModel LaserModel;
	CModel SpaceBoxModel;
	glm::mat4 SpaceBoxModelMatrix = glm::mat4(1.f);

	// The asteroid pool for constant-time acces and no instatiations in-game.
	CEntityPool<CAsteroid, 6000> AsteroidPool;
	// Pas le temps...
	// CEntityPool<CLaser, 200> LaserPool;

	// Physics.
	rp3d::PhysicsCommon PhysicsCommon;
	rp3d::PhysicsWorld* PhysicsWorld = nullptr;
	CCollisionListener CollisionListener;
	float const PhysicsDt = 1.f / 60.f;
	float TimeAccumulator = 0.f;
	float InterpolationFactor = 0.f;

	// Rendering stuff.
	GLFWwindow* const Window = nullptr;
	glm::mat4 ProjectionMatrix;
	CCamera<20> Camera = CCamera<20>(CCameraTarget(), 0.f, 1.f);
	glm::vec3 const LightPosition = glm::vec3(0.f, 1000.f, 0.f);
	glm::vec3 const LightColor = glm::vec3(1.f, 1.f, 1.f);

	// Time tracking.
	float _Time = 0.f;
};