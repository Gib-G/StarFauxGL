#pragma once
#include "Entity.h"
#include "Axes.h"

class CCameraTarget;
class CWorld;

class CArwing : public CEntity
{
	using CParent = CEntity;
public:
	CArwing(CWorld* const World, CModel* const Model = nullptr);
	virtual void InitializeRigidBody(rp3d::PhysicsCommon&, rp3d::PhysicsWorld* const) override;

	// !! CLAMPING DONE IN UPDATE !!
	// Linear motion. These only change the linear velocity. Motion is only applied when CArwing::Update is called.
	// Dt = fixed physics simulation time step.
	void Accelerate(float const Dt);
	void Deccelerate(float const Dt);

	// !! CLAMPING DONE IN UPDATE !!
	// Angular motion. These only change the angular velocities. Motion is only applied when CArwing::Update is called.
	// Dt = fixed physics simulation time step.
	void TurnLeft(float const Dt);
	void TurnRight(float const Dt);
	void GoUp(float const Dt);
	void GoDown(float const Dt);

	// The Dt to pass here is the fixed one used for the physics simulation.
	virtual void Update(float const Dt) override;

	// Normalized local vectors expressed in the world frame.
	glm::vec3 GetForwardAxis() const;
	glm::vec3 GetUpAxis() const;
	glm::vec3 GetRightAxis() const;

	// Returns info about the current position and orientation of the Arwing in the SCameraTarget format
	// manipulated by the CCamera class.
	CCameraTarget GetCameraTarget() const;

private:
	// The motion of the Arwing is not handled by the physics engine: the Arwing has a kinematic rigid body.
	// Linear motion params (in SI units, absolute values).
	float const LinearAcceleration = 100.f;
	float const LinearDamping = 5.f;
	float const LinearDecceleration = 30.f;
	float const MaxLinearVelocity = 400.f;
	float const MinLinearVelocity = 40.f;
	float LinearVelocity = 40.f;
	float const Mass = 850.f;

	// Angular motion params per local axis (absolute values).
	// Units : deg/s or deg/s^2.
	float const AngularAccelerations[Dim] = { 270.f, 270.f, 270.f };
	float const AngularDampings[Dim] = { 120.f, 120.f, 120.f };
	float const MaxAngularVelocities[Dim] = { 70.f, 70.f, 70.f };
	float const MinAngularVelocities[Dim] = { 0.f, 0.f, 0.f };
	float AngularVelocities[Dim] = { 0.f, 0.f, 0.f };

	// Normalized local vectors expressed in the Arwing frame.
	glm::vec3 constexpr GetLocalForwardAxis() const;
	glm::vec3 constexpr GetLocalUpAxis() const;
	glm::vec3 constexpr GetLocalRightAxis() const;
};