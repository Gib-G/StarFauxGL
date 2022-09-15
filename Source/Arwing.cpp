#include "Arwing.h"
#include "Camera.h"
#include "StringUtil.h"
#include "World.h"

CArwing::CArwing(CWorld* const World, CModel* const Model) : CParent(World, EEntityType::Arwing, Model)
{
	// Checks.
	assert(World);
	assert(Mass >= 0.f);
	assert(LinearAcceleration >= 0.f);
	assert(LinearDamping >= 0.f);
	assert(LinearDeceleration >= 0.f);
	assert(MinLinearVelocity >= 0.f);
	assert(MinLinearVelocity <= LinearVelocity && LinearVelocity <= MaxLinearVelocity);

	for (int iAxis = 0; iAxis < Dim; iAxis++)
	{
		assert(AngularAccelerations[iAxis] >= 0.f);
		assert(AngularDampings[iAxis] >= 0.f);

		float const minAngularVelocity = MinAngularVelocities[iAxis];
		float const maxAngularVelocity = MaxAngularVelocities[iAxis];
		assert(minAngularVelocity >= 0.f);
		assert(maxAngularVelocity >= 0.f);

		float const angularVelocity = AngularVelocities[iAxis];

		if (angularVelocity >= 0.f) assert(minAngularVelocity <= angularVelocity && angularVelocity <= maxAngularVelocity);
		else assert(-maxAngularVelocity <= angularVelocity && angularVelocity <= -minAngularVelocity);
	}

	// Initial transform. Scaling is performed last only when drawing the model (cf. CEntity::Draw).
	ModelMatrix = glm::mat4(1.f);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f));
	Size = 7.f; // Biggest extent: 7m (probably from front to back).
}

void CArwing::InitializeRigidBody(rp3d::PhysicsCommon& PhysicsCommon, rp3d::PhysicsWorld* const PhysicsWorld)
{
	using namespace rp3d;
	if (!Model) { RigidBody = nullptr; return; }

	Transform transform; transform.setFromOpenGL(reinterpret_cast<decimal*>(&ModelMatrix));
	RigidBody = PhysicsWorld->createRigidBody(transform);

	Vector3 const boxHalfExtents = 0.5f * NormalizingScalingFactor * Size * Model->GetAABB().GetLength();
	BoxShape* box = PhysicsCommon.createBoxShape(boxHalfExtents);
	RigidBody->addCollider(box, rp3d::Transform::identity());

	RigidBody->setType(BodyType::KINEMATIC);
	// RigidBody->setMass(Mass);

	RigidBody->setUserData(this);
}

void CArwing::Update(float const Dt)
{
	// Responding to keyboard inputs.
	if (ShouldAccelerate) Accelerate(Dt);
	if (ShouldDecelerate) Decelerate(Dt);
	if (ShouldGoUp) GoUp(Dt);
	if (ShouldGoDown) GoDown(Dt);
	if (ShouldTurnLeft) TurnLeft(Dt);
	if (ShouldTurnRight) TurnRight(Dt);

	glm::vec3 const& localForwardAxis = GetLocalForwardAxis();
	
	// ANGULAR MOTION.
	// Rotating (pitch, then roll, then yaw).
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(AngularVelocities[int(EDirection::Right)] * Dt), GetLocalRightAxis());
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(AngularVelocities[int(EDirection::Forward)] * Dt), localForwardAxis);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(AngularVelocities[int(EDirection::Up)] * Dt), GetLocalUpAxis());

	for (int iAxis = 0; iAxis < Dim; iAxis++)
	{
		// Decaying angular velocities.
		float& angularVelocity = AngularVelocities[iAxis];
		if (angularVelocity > 0.f) angularVelocity -= AngularDampings[iAxis] * Dt;
		else if(angularVelocity < 0.f) angularVelocity += AngularDampings[iAxis] * Dt;

		// Clamping angular velocities.
		if (angularVelocity >= 0.f) angularVelocity = std::clamp(angularVelocity, MinAngularVelocities[iAxis], MaxAngularVelocities[iAxis]);
		else angularVelocity = std::clamp(angularVelocity, -MaxAngularVelocities[iAxis], -MinAngularVelocities[iAxis]);
	}

	// LINEAR MOTION.
	// Translating.
	ResetScale();
	ModelMatrix = glm::translate(ModelMatrix, LinearVelocity * Dt * localForwardAxis);

	// Decaying linear velocity.
	LinearVelocity -= LinearDamping * Dt;

	// Clamping linear velocity.
	LinearVelocity = std::clamp(LinearVelocity, MinLinearVelocity, MaxLinearVelocity);

	// Passing the new transform to the kinematic rigid body.
	using namespace rp3d;
	if (!RigidBody) return;
	Transform transform; transform.setFromOpenGL(reinterpret_cast<decimal*>(&ModelMatrix));
	RigidBody->setTransform(transform);
}

void CArwing::Accelerate(float const Dt)
{
	LinearVelocity = std::clamp(LinearVelocity + LinearAcceleration * Dt, MinLinearVelocity, MaxLinearVelocity);
}

void CArwing::Decelerate(float const Dt)
{
	LinearVelocity = std::clamp(LinearVelocity - LinearDeceleration * Dt, MinLinearVelocity, MaxLinearVelocity);
}

void CArwing::TurnLeft(float const Dt)
{
	float& rollSpeed = AngularVelocities[int(EDirection::Forward)];
	rollSpeed -= AngularAccelerations[int(EDirection::Forward)] * Dt;
	if (rollSpeed <= 0.f) rollSpeed = std::clamp(rollSpeed, -MaxAngularVelocities[int(EDirection::Forward)], -MinAngularVelocities[int(EDirection::Forward)]);
	else rollSpeed = std::clamp(rollSpeed, MinAngularVelocities[int(EDirection::Forward)], MaxAngularVelocities[int(EDirection::Forward)]);

	float& yawSpeed = AngularVelocities[int(EDirection::Up)];
	yawSpeed += AngularAccelerations[int(EDirection::Up)] * Dt;
	if (yawSpeed <= 0.f) yawSpeed = std::clamp(yawSpeed, -MaxAngularVelocities[int(EDirection::Up)], -MinAngularVelocities[int(EDirection::Up)]);
	else yawSpeed = std::clamp(yawSpeed, MinAngularVelocities[int(EDirection::Up)], MaxAngularVelocities[int(EDirection::Up)]);
}

void CArwing::TurnRight(float const Dt)
{
	float& rollSpeed = AngularVelocities[int(EDirection::Forward)];
	rollSpeed += AngularAccelerations[int(EDirection::Forward)] * Dt;
	if (rollSpeed <= 0.f) rollSpeed = std::clamp(rollSpeed, -MaxAngularVelocities[int(EDirection::Forward)], -MinAngularVelocities[int(EDirection::Forward)]);
	else rollSpeed = std::clamp(rollSpeed, MinAngularVelocities[int(EDirection::Forward)], MaxAngularVelocities[int(EDirection::Forward)]);

	float& yawSpeed = AngularVelocities[int(EDirection::Up)];
	yawSpeed -= AngularAccelerations[int(EDirection::Up)] * Dt;
	if (yawSpeed <= 0.f) yawSpeed = std::clamp(yawSpeed, -MaxAngularVelocities[int(EDirection::Up)], -MinAngularVelocities[int(EDirection::Up)]);
	else yawSpeed = std::clamp(yawSpeed, MinAngularVelocities[int(EDirection::Up)], MaxAngularVelocities[int(EDirection::Up)]);
}

void CArwing::GoUp(float const Dt)
{
	float& pitchSpeed = AngularVelocities[int(EDirection::Right)];
	pitchSpeed += AngularAccelerations[int(EDirection::Right)] * Dt;
	if (pitchSpeed <= 0.f) pitchSpeed = std::clamp(pitchSpeed, -MaxAngularVelocities[int(EDirection::Right)], -MinAngularVelocities[int(EDirection::Right)]);
	else pitchSpeed = std::clamp(pitchSpeed, MinAngularVelocities[int(EDirection::Right)], MaxAngularVelocities[int(EDirection::Right)]);
}

void CArwing::GoDown(float const Dt)
{
	float& pitchSpeed = AngularVelocities[int(EDirection::Right)];
	pitchSpeed -= AngularAccelerations[int(EDirection::Right)] * Dt;
	if (pitchSpeed <= 0.f) pitchSpeed = std::clamp(pitchSpeed, -MaxAngularVelocities[int(EDirection::Right)], -MinAngularVelocities[int(EDirection::Right)]);
	else pitchSpeed = std::clamp(pitchSpeed, MinAngularVelocities[int(EDirection::Right)], MaxAngularVelocities[int(EDirection::Right)]);
}

glm::vec3 CArwing::GetForwardAxis() const
{
	return glm::normalize(glm::vec3(ModelMatrix[1]));
}

glm::vec3 CArwing::GetUpAxis() const
{
	return -glm::normalize(glm::vec3(ModelMatrix[2]));
}

glm::vec3 CArwing::GetRightAxis() const
{
	return -glm::normalize(glm::vec3(ModelMatrix[0]));
}

CCameraTarget CArwing::GetCameraTarget() const
{
	return CCameraTarget(GetPosition(), GetForwardAxis(), GetUpAxis());
}

glm::vec3 constexpr CArwing::GetLocalForwardAxis() const { return glm::vec3(0.f, 1.f, 0.f); }
glm::vec3 constexpr CArwing::GetLocalUpAxis() const { return glm::vec3(0.f, 0.f, -1.f); }
glm::vec3 constexpr CArwing::GetLocalRightAxis() const { return glm::vec3(-1.f, 0.f, 0.f); }