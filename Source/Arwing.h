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
	// Fetchs them from the model matrix, not the transform of the rigid body.
	glm::vec3 GetForwardAxis() const;
	glm::vec3 GetUpAxis() const;
	glm::vec3 GetRightAxis() const;

	// Returns info about the current position and orientation of the Arwing in the SCameraTarget format
	// manipulated by the CCamera class.
	CCameraTarget GetCameraTarget() const;

private:
	// Linear motion params (in SI units, absolute values).
	float const Mass = 850.f;
	float const LinearAcceleration = 0.f;
	float const LinearDamping = 5.f;
	float const LinearDecceleration = 30.f;
	float const MaxLinearSpeed = 400.f;
	float const MinLinearSpeed = 0.f;

	// Angular motion params per local axis (absolute values). Order: see EAxis and EDirection.
	// Units : rad/s or rad/s^2.
	float const AngularAccelerations[Dim] = { 11.f, 11.f, 13.f };
	float const AngularDampings[Dim] = { 6.f, 6.f, 7.f };
	float const MaxAngularSpeeds[Dim] = { 1.2f, 1.2f, 1.5f };
	float const MinAngularSpeeds[Dim] = { 0.f, 0.f, 0.f };
};