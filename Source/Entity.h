#pragma once
#include "Types.h"
#include "Model.h"
#include <reactphysics3d/reactphysics3d.h> 

class CWorld;

enum class EEntityType : uint8_t { Arwing = 0, Asteroid, Laser, Unknown, EnumCount };

// Generic game entity class (entity-component stuff).
class CEntity
{
public:
	CEntity(CWorld* const World, EEntityType const Type, CModel* const Model = nullptr);
	
	void SetModel(CModel* const);
	virtual void InitializeRigidBody(rp3d::PhysicsCommon&, rp3d::PhysicsWorld* const);

	EEntityType GetType() const;
	glm::vec3 GetPosition() const;

	virtual void Update(float const Dt) = 0;
	void Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor);

	void SetActive(bool const IsActive);
	bool IsActive() const;

	// Could be defined only in classes that always have a rigid body to avoid checking that RigidBody != nullptr all the time.
	void UpdateModelMatrixFromRigidBody(float const InterpolationFactor);

private:
	// The world the entity belongs to.
	CWorld* const World = nullptr;

	// Inactive entities are neither updated, nor physically simulated, nor rendered.
	bool Active = true;
	EEntityType const Type = EEntityType::Unknown;

	rp3d::Transform PreviousTransform = rp3d::Transform::identity();

	float NormalizingScalingFactor = 1.f;
	void ResetScale(); // Normalizes the model matrix's orientation vectors.

protected:
	int Hp = 500;

	CModel* Model = nullptr;
	glm::mat4 ModelMatrix = glm::mat4(1.f);
	// Resource managed by rp3d::PhysicsCommon. Do not call delete on this pointer!!
	rp3d::RigidBody* RigidBody = nullptr;

	float Size = 1.f; // In m.

	bool DrawTextures = true;
};

class CAsteroid : public CEntity
{
public:
	CAsteroid(CWorld* const World, CModel* Model = nullptr);

	virtual void InitializeRigidBody(rp3d::PhysicsCommon&, rp3d::PhysicsWorld* const) override;

	virtual void Update(float const Dt) override;

	struct SParams
	{
		SParams() = default;
		// In m.
		glm::vec3 PlayerPosition = glm::vec3(0.f);
		float MinSpawnDistanceFromPlayer = 400.f;
		float MaxSpawnDistanceFromPlayer = 1000.f;
		
		// In m.
		float MinSize = 1.f;
		float MaxSize = 10.f;

		// In rad/s and m/s.
		float MinAngularVelocity = 0.05f, MaxAngularVelocity = 4.f;
		float MinLinearVelocity = 0.5f, MaxLinearVelocity = 50.f;
	};
	// Randomizes the asteroid's location around the player, its velocities, initial transform and size.
	void Randomize(SParams const& Params = SParams());

private:
	glm::vec3 LinearVelocity;
	glm::vec3 AngularVelocity;
};

class CLaser : public CEntity
{

};