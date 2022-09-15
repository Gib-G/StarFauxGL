#pragma once
#include "Types.h"
#include "Model.h"
#include <reactphysics3d/reactphysics3d.h> 

class CWorld;

enum class EEntityType : uint8_t { Arwing = 0, Asteroid, Laser, Unknown, EnumCount };
static constexpr char const* s_EntityNames[int(EEntityType::EnumCount)] = {"Arwing", "Asteroid", "Laser", "Unknown"};

// Generic game entity class (entity-component stuff).
class CEntity
{
public:
	CEntity(CWorld* const World, EEntityType const Type, CModel* const Model = nullptr);
	
	void SetModel(CModel* const);
	virtual void InitializeRigidBody(rp3d::PhysicsCommon&, rp3d::PhysicsWorld* const);
	void DestroyRigidBody(rp3d::PhysicsWorld* const);

	EEntityType GetType() const;
	glm::vec3 GetPosition() const;

	virtual void Update(float const Dt) = 0;
	void Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor);

	void SetActive(bool const IsActive);
	bool IsActive() const;

	virtual void OnCollision(CEntity const& CollidedWith) {};

	// Could be defined only in classes that always have a rigid body to avoid checking that RigidBody != nullptr all the time.
	void UpdateModelMatrixFromRigidBody(float const InterpolationFactor);

private:
	// Inactive entities are neither updated, nor physically simulated, nor rendered.
	bool Active = true;
	EEntityType const Type = EEntityType::Unknown;

	rp3d::Transform PreviousTransform = rp3d::Transform::identity();

protected:
	// The world the entity belongs to.
	CWorld* const World = nullptr;

	int Hp = 500;

	// The entity has no ownership over its model.
	// Do not call delete on this pointer within CEntity or its derived classes.
	CModel* Model = nullptr;
	glm::mat4 ModelMatrix = glm::mat4(1.f);
	// Resource managed by rp3d::PhysicsCommon. Do not call delete on this pointer!!
	rp3d::RigidBody* RigidBody = nullptr;

	float NormalizingScalingFactor = 1.f;
	float Size = 1.f; // In m.

	bool DrawTextures = true;

	void ResetScale(); // Normalizes the model matrix's orientation vectors.

public:
	// Should this really be virtual?
	virtual CEntity& operator=(CEntity const& Other);
};

class CAsteroid : public CEntity
{
public:
	CAsteroid(CWorld* const World, CModel* Model = nullptr);

	virtual void InitializeRigidBody(rp3d::PhysicsCommon&, rp3d::PhysicsWorld* const) override;

	virtual void Update(float const Dt) override;

	virtual void OnCollision(CEntity const& CollidedWith) override;

	struct SParams
	{
		SParams() = default;
		// In m.
		glm::vec3 PlayerPosition = glm::vec3(0.f);
		float MinSpawnDistanceFromPlayer = 500.f;
		float MaxSpawnDistanceFromPlayer = 1000.f;
		
		// In m.
		float MinSize = 3.f;
		float MaxSize = 70.f;
		// In kg.
		float MinMass = 100.f;
		float MaxMass = 10000.f;

		// In rad/s and m/s.
		float MinAngularVelocity = 0.05f, MaxAngularVelocity = 4.f;
		float MinLinearVelocity = 0.5f, MaxLinearVelocity = 50.f;
	};
	// Randomizes the asteroid's location around the player, its velocities, initial transform and size.
	void Randomize(SParams const& Params = SParams());

private:
	float Mass = 2000.f;

	rp3d::Vector3 LinearVelocity;
	rp3d::Vector3 AngularVelocity;

	// Asteroids despawn when they are this far from the Arwing.
	float const DespawnDistance = 10000.f;
};

// J'ai pas eu le temps... :(
// We are supposed to be able to shot laser projectiles with the Arwing.
class CLaser : public CEntity
{

};