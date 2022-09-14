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
	assert(LinearDecceleration >= 0.f);
	assert(MinLinearSpeed >= 0.f);
	assert(MaxLinearSpeed >= MinLinearSpeed);

	for (int iAxis = 0; iAxis < Dim; iAxis++)
	{
		assert(AngularAccelerations[iAxis] >= 0.f);
		assert(AngularDampings[iAxis] >= 0.f);
		float const minAngularSpeed = MinAngularSpeeds[iAxis];
		float const maxAngularSpeed = MaxAngularSpeeds[iAxis];
		assert(minAngularSpeed >= 0.f);
		assert(maxAngularSpeed >= 0.f);
		assert(minAngularSpeed <= maxAngularSpeed);
	}
	// Initial transform. Scaling is performed last only when drawing the model (cf. CEntity::Draw).
	ModelMatrix = glm::mat4(1.f);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f));
	Size = 7.f; // Biggest extent: 7m (probably from front to back).
}

void CArwing::InitializeRigidBody(rp3d::PhysicsCommon& PhysicsCommon, rp3d::PhysicsWorld* const PhysicsWorld)
{
	if (!Model) { RigidBody = nullptr; return; }
	rp3d::Transform transform; transform.setFromOpenGL(reinterpret_cast<rp3d::decimal*>(&ModelMatrix));
	RigidBody = PhysicsWorld->createRigidBody(transform);
	rp3d::Vector3 const boxHalfExtents = 0.5f * Model->GetAABB().GetLength();
	rp3d::BoxShape* box = PhysicsCommon.createBoxShape(boxHalfExtents);
	RigidBody->addCollider(box, rp3d::Transform::identity());

	RigidBody->setMass(Mass);
	RigidBody->setLinearDamping(LinearDamping);
}

// The Dt to pass here is the fixed one used for the physics simulation.
void CArwing::Update(float const Dt)
{
	if (!RigidBody) return;
	using namespace rp3d;
	// Clamping and decaying angular velocities.
	Vector3 angularVelocity = RigidBody->getAngularVelocity();
	glm::vec3 const f = GetForwardAxis(); Vector3 const fArwing(f.x, f.y, f.z);
	glm::vec3 const u = GetUpAxis(); Vector3 const uArwing(u.x, u.y, u.z);
	glm::vec3 const r = GetRightAxis(); Vector3 const rArwing(r.x, r.y, r.z);
	float rollSpeed = angularVelocity.dot(fArwing);
	if(rollSpeed < 0.f) rollSpeed += AngularDampings[int(EDirection::Forward)] * Dt;
	else if(rollSpeed > 0.f) rollSpeed -= AngularDampings[int(EDirection::Forward)] * Dt;
	float yawSpeed = angularVelocity.dot(uArwing);
	if(yawSpeed < 0.f) yawSpeed += AngularDampings[int(EDirection::Up)] * Dt;
	else if(yawSpeed > 0.f) yawSpeed -= AngularDampings[int(EDirection::Up)] * Dt;
	float pitchSpeed = angularVelocity.dot(rArwing);
	if(pitchSpeed < 0.f) pitchSpeed += AngularDampings[int(EDirection::Right)] * Dt;
	else if(pitchSpeed > 0.f) pitchSpeed -= AngularDampings[int(EDirection::Right)] * Dt;
	// Clamping.
	if (rollSpeed <= 0.f) rollSpeed = std::clamp(rollSpeed, -MaxAngularSpeeds[int(EDirection::Forward)], -MinAngularSpeeds[int(EDirection::Forward)]);
	else rollSpeed = std::clamp(rollSpeed, MinAngularSpeeds[int(EDirection::Forward)], MaxAngularSpeeds[int(EDirection::Forward)]);
	if (yawSpeed <= 0.f) yawSpeed = std::clamp(yawSpeed, -MaxAngularSpeeds[int(EDirection::Up)], -MinAngularSpeeds[int(EDirection::Up)]);
	else yawSpeed = std::clamp(yawSpeed, MinAngularSpeeds[int(EDirection::Up)], MaxAngularSpeeds[int(EDirection::Up)]);
	if (pitchSpeed <= 0.f) pitchSpeed = std::clamp(pitchSpeed, -MaxAngularSpeeds[int(EDirection::Right)], -MinAngularSpeeds[int(EDirection::Right)]);
	else pitchSpeed = std::clamp(pitchSpeed, MinAngularSpeeds[int(EDirection::Right)], MaxAngularSpeeds[int(EDirection::Right)]);
	RigidBody->setAngularVelocity(rollSpeed * fArwing + yawSpeed * uArwing + pitchSpeed * rArwing);

	// Clamping linear velocity. Damping is managed by the physics engine.
	Vector3 velocityVector = RigidBody->getLinearVelocity();
	float const velocity = std::clamp(velocityVector.length(), MinLinearSpeed, MaxLinearSpeed);
	velocityVector.normalize();
	RigidBody->setLinearVelocity(velocity * velocityVector);
}

void CArwing::Accelerate(float const dt)
{
	using namespace rp3d;
	Vector3 velocity = RigidBody->getLinearVelocity();
	glm::vec3 const& fTemp = GetForwardAxis(); Vector3 f(fTemp.x, fTemp.y, fTemp.z);
	velocity += (LinearAcceleration * dt) * f;
	RigidBody->setLinearVelocity(velocity);
}

void CArwing::Deccelerate(float const dt)
{
	using namespace rp3d;
	Vector3 velocity = RigidBody->getLinearVelocity();
	glm::vec3 const& fTemp = GetForwardAxis(); Vector3 f(fTemp.x, fTemp.y, fTemp.z);
	velocity -= (LinearDecceleration * dt) * f;
	RigidBody->setLinearVelocity(velocity);
}

void CArwing::TurnLeft(float const dt)
{
	using namespace rp3d;
	Vector3 velocity = RigidBody->getAngularVelocity();
	glm::vec3 const& fTemp = GetForwardAxis(); Vector3 f(fTemp.x, fTemp.y, fTemp.z);
	glm::vec3 const& uTemp = GetUpAxis(); Vector3 u(uTemp.x, uTemp.y, uTemp.z);
	velocity += dt * (AngularAccelerations[int(EDirection::Up)] * u - AngularAccelerations[int(EDirection::Forward)] * f);
	RigidBody->setAngularVelocity(velocity);
}

void CArwing::TurnRight(float const dt)
{
	using namespace rp3d;
	Vector3 velocity = RigidBody->getAngularVelocity();
	glm::vec3 const& fTemp = GetForwardAxis(); Vector3 f(fTemp.x, fTemp.y, fTemp.z);
	glm::vec3 const& uTemp = GetUpAxis(); Vector3 u(uTemp.x, uTemp.y, uTemp.z);
	velocity += dt * (AngularAccelerations[int(EDirection::Forward)] * f - AngularAccelerations[int(EDirection::Up)] * u);
	RigidBody->setAngularVelocity(velocity);
}

void CArwing::GoUp(float const dt)
{
	using namespace rp3d;
	Vector3 velocity = RigidBody->getAngularVelocity();
	glm::vec3 const& rTemp = GetRightAxis(); Vector3 r(rTemp.x, rTemp.y, rTemp.z);
	velocity += AngularAccelerations[int(EDirection::Right)] * dt * r;
	RigidBody->setAngularVelocity(velocity);
}

void CArwing::GoDown(float const dt)
{
	using namespace rp3d;
	Vector3 velocity = RigidBody->getAngularVelocity();
	glm::vec3 const& rTemp = GetRightAxis(); Vector3 r(rTemp.x, rTemp.y, rTemp.z);
	velocity -= AngularAccelerations[int(EDirection::Right)] * dt * r;
	RigidBody->setAngularVelocity(velocity);
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