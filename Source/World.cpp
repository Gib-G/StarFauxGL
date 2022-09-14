#include "World.h"

CWorld::CWorld(float const AspectRatio)
{
	ProjectionMatrix = glm::perspective(45.f, AspectRatio, 1.f, 350000.f);

	PhysicsWorld = PhysicsCommon.createPhysicsWorld();
	PhysicsWorld->setIsGravityEnabled(false);

	ArwingModel.Load(ROOT_DIR"Resources\\Meshes\\Arwing\\arwing_starlink.fbx");
	//AsteroidModel.Load(ROOT_DIR"Resources\\Meshes\\Asteroid\\asteroid.obj");
	LaserModel.Load(ROOT_DIR"Resources\\Meshes\\Laser\\laser.obj");
	SpaceBoxModel.Load(ROOT_DIR"Resources\\Meshes\\SpaceBox\\space.obj");
	SpaceBoxModelMatrix = glm::scale(SpaceBoxModelMatrix, glm::vec3(WorldRadius));

	Arwing.SetModel(&ArwingModel);
	Arwing.InitializeRigidBody(PhysicsCommon, PhysicsWorld);

	//CAsteroid asteroid(this, &AsteroidModel);
	//AsteroidPool.FillWith(asteroid);
}

void CWorld::Update(float const Dt)
{
	// Regular updates.
	Arwing.Update(PhysicsDt);
	Camera.UpdateViewMatrix(Arwing.GetCameraTarget()); // À mettre plus bas peut-être...
	/*
	glm::vec3 const& arwingPosition = Arwing.GetPosition();
	for (uint16_t i = 0; i < AsteroidPool.NumberOfEntities; i++)
	{
		CAsteroid& asteroid = AsteroidPool.Entities[i];
		if (asteroid.IsActive())
		{
			asteroid.Update(Dt);
			// Despawn if too far.
			if (glm::length(asteroid.GetPosition() - arwingPosition) >= AsteroidDespawnDistance) asteroid.SetActive(false);
			if (!asteroid.IsActive())
			{
				AsteroidPool.FirstInactiveEntity = std::min(AsteroidPool.FirstInactiveEntity, i);
			}
		}
	}
	*/
	/*
	for (uint16_t i = 0; i < LasersPool.NumberOfEntities; i++)
	{
		CLaser& laser = LasersPool.Entities[i];
		if (laser.IsActive())
		{
			laser.Update(Dt);
			// Despawn if too far.
			if (glm::length(laser.GetPosition() - arwingPosition) >= LaserDespawnDistance) laser.SetActive(false);
			if (!laser.IsActive())
			{
				LasersPool.FirstInactiveEntity = std::min(LasersPool.FirstInactiveEntity, i);
			}
		}
	}
	*/
	// Physics update.
	TimeAccumulator += Dt;
	while (TimeAccumulator >= PhysicsDt)
	{
		PhysicsWorld->update(PhysicsDt);
		TimeAccumulator -= PhysicsDt;
	}
	float const interpolationFactor = TimeAccumulator / PhysicsDt;
	assert(0.f <= interpolationFactor && interpolationFactor <= 1.f);

	Arwing.UpdateModelMatrixFromRigidBody(interpolationFactor);
	// Looping again to immediately apply the changes from the physics update to the game entitites.
	// We could do something more optimal but hey...
	/*
	for (CAsteroid& asteroid : AsteroidPool.Entities)
	{
		if (asteroid.IsActive()) asteroid.UpdateModelMatrixFromRigidBody(interpolationFactor);
	}
	*/
	/*
	for (CLaser& laser : LasersPool.Entities)
	{
		if (laser.IsActive()) laser.UpdateModelMatrixFromRigidBody(interpolationFactor);
	}
	*/
	_Time += Dt;
	if (_Time >= AsteroidSpawnTime) { SpawnAsteroid(); _Time = 0.f; }
}

void CWorld::Render()
{
	glm::vec3 const& cameraPosition = Camera.GetPosition();
	glm::mat4 const& viewMatrix = Camera.GetViewMatrix();

	SpaceBoxModel.Draw(cameraPosition, SpaceBoxModelMatrix, viewMatrix, ProjectionMatrix, LightPosition, LightColor);
	Arwing.Draw(cameraPosition, viewMatrix, ProjectionMatrix, LightPosition, LightColor);
	/*
	for (CAsteroid& asteroid : AsteroidPool.Entities) asteroid.Draw(cameraPosition, viewMatrix, ProjectionMatrix, LightPosition, LightColor);
	*/
	/*
	for (CLaser& laser : LasersPool.Entities)
	{
		if (laser.IsActive()) laser.UpdateModelMatrixFromRigidBody(interpolationFactor);
	}
	*/
}

void CWorld::SpawnAsteroid()
{
	/*
	CAsteroid& asteroid = *AsteroidPool.GetInactiveEntity();
	asteroid.Randomize();
	asteroid.SetActive(true);
	*/
}