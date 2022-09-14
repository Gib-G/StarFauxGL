#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include "Model.h"
#include "Arwing.h"
#include "Camera.h"
#include "EntityPool.h"

// Basically a container for everything in the game.
class CWorld
{
public:
	CWorld();

	// Dt = dynamic game delta time.
	void Update(float const Dt);
	void Render();

	void SpawnAsteroid();

	CArwing Arwing = CArwing(this);

private:
	// Used to scale the skybox (SpaceBox, 1 * 1 * 1 m cube).
	float const WorldRadius = 2.e5f;

	// Time between two asteroid spawns.
	float AsteroidSpawnTime = 4.f; // In s.
	// Asteroids and lasers despawn (get deactivated) when they are beyond this distance from the Arwing.
	float const AsteroidDespawnDistance = 1500.f;
	float const LaserDespawnDistance = 1500.f;

	CModel ArwingModel;
	CModel AsteroidModel;
	CModel LaserModel;
	CModel SpaceBoxModel;
	glm::mat4 SpaceBoxModelMatrix = glm::mat4(1.f);

	CEntityPool<CAsteroid, 200> AsteroidPool;
	//SEntityPool<CLaser, 100> LaserPool;

	// Physics.
	rp3d::PhysicsCommon PhysicsCommon;
	rp3d::PhysicsWorld* PhysicsWorld = nullptr;
	float const PhysicsDt = 1.f / 60.f;
	float TimeAccumulator = 0.f;

	// Rendering stuff.
	glm::mat4 const ProjectionMatrix = glm::perspective(45.f, 1.3f, 1.f, 350000.f);
	CCamera<20> Camera = CCamera<20>(CCameraTarget(), 0.f, 1.f);
	glm::vec3 const LightPosition = glm::vec3(0.f, 1000.f, 0.f);
	glm::vec3 const LightColor = glm::vec3(1.f, 1.f, 1.f);

	// Time tracking.
	float _Time = 0.f;
};