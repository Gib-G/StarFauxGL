#include "Arwing.h"
#include "Camera.h"

CArwing::CArwing(bool const LoadModelNow)
{
	// Loading model.
	if (LoadModelNow) { if (!Model.Load("meshes/arwing/arwing_starlink.fbx")) std::cout << "Failed to load Arwing 3D model!\n"; }

	// Initial transform. Scaling is performed last only when drawing the model.
	ModelMatrix = glm::mat4(1.f);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f));

	// Checks.
	assert(LinearAcceleration >= 0.f);
	assert(LinearDrag >= 0.f);
	assert(LinearDecceleration >= 0.f);
	assert(MinLinearSpeed >= 0.f);
	assert(MaxLinearSpeed >= MinLinearSpeed);
	assert(MinLinearSpeed <= LinearSpeed && LinearSpeed <= MaxLinearSpeed);

	for (int iAxis = 0; iAxis < int(ELocalAxis::EnumCount); iAxis++)
	{
		assert(AngularAccelerations[iAxis] >= 0.f);
		assert(AngularDeccelerations[iAxis] >= 0.f);
		float const minAngularSpeed = MinAngularSpeeds[iAxis];
		float const maxAngularSpeed = MaxAngularSpeeds[iAxis];
		assert(minAngularSpeed >= 0.f);
		assert(maxAngularSpeed >= 0.f);
		float const angularSpeed = AngularSpeeds[iAxis];
		if (angularSpeed >= 0.f) assert(minAngularSpeed <= angularSpeed && angularSpeed <= maxAngularSpeed);
		else assert(-maxAngularSpeed <= angularSpeed && angularSpeed <= -minAngularSpeed);
	}
}

void CArwing::LoadModel()
{
	if (!Model.Load("meshes/arwing/arwing_starlink.fbx")) std::cout << "Failed to load Arwing 3D model!\n";
}

void CArwing::Draw(glm::vec3 const& CameraPosition, glm::mat4 const& View, glm::mat4 const& Projection, glm::vec3 const& LightPosition, glm::vec3 const& LightColor, bool ForceAmbient)
{
	// Scaling only when drawing so it is the last transformation applied to the model.
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.016f, 0.016f, 0.016f));
	Model.Draw(CameraPosition, ModelMatrix, View, Projection, LightPosition, LightColor, ForceAmbient);
	NormalizeModelMatrix();
}

void CArwing::Move(float const dt)
{
	// ANGULAR MOTION.
	// Rotating (pitch, then roll, then yaw).
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(AngularSpeeds[int(ELocalAxis::Right)] * dt), LocalAxes[int(ELocalAxis::Right)]);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(AngularSpeeds[int(ELocalAxis::Forward)] * dt), LocalAxes[int(ELocalAxis::Forward)]);
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(AngularSpeeds[int(ELocalAxis::Up)] * dt), LocalAxes[int(ELocalAxis::Up)]);

	// Decaying angular velocities.
	for (int iAxis = 0; iAxis < int(ELocalAxis::EnumCount); iAxis++)
	{
		float& angularSpeed = AngularSpeeds[iAxis];
		if (angularSpeed >= 0.f)
		{
			angularSpeed -= AngularDeccelerations[iAxis] * dt;
			angularSpeed = std::clamp(angularSpeed, MinAngularSpeeds[iAxis], MaxAngularSpeeds[iAxis]);
		}
		else
		{
			angularSpeed += AngularDeccelerations[iAxis] * dt;
			angularSpeed = std::clamp(angularSpeed, -MaxAngularSpeeds[iAxis], -MinAngularSpeeds[iAxis]);
		}
	}

	// LINEAR MOTION.
	// Translating.
	NormalizeModelMatrix();
	ModelMatrix = glm::translate(ModelMatrix, LinearSpeed * dt * LocalAxes[int(ELocalAxis::Forward)]);

	// Decaying linear velocity.
	LinearSpeed -= LinearDrag * dt;
	LinearSpeed = std::clamp(LinearSpeed, MinLinearSpeed, MaxLinearSpeed);
}

void CArwing::Accelerate(float const dt)
{
	LinearSpeed = std::clamp(LinearSpeed + LinearAcceleration * dt, MinLinearSpeed, MaxLinearSpeed);
}

void CArwing::Deccelerate(float const dt)
{
	LinearSpeed = std::clamp(LinearSpeed - LinearDecceleration * dt, MinLinearSpeed, MaxLinearSpeed);
}

void CArwing::TurnLeft(float const dt)
{
	float& rollSpeed = AngularSpeeds[int(ELocalAxis::Forward)];
	rollSpeed -= AngularAccelerations[int(ELocalAxis::Forward)] * dt;
	if (rollSpeed <= 0.f) rollSpeed = std::clamp(rollSpeed, -MaxAngularSpeeds[int(ELocalAxis::Forward)], -MinAngularSpeeds[int(ELocalAxis::Forward)]);
	else rollSpeed = std::clamp(rollSpeed, MinAngularSpeeds[int(ELocalAxis::Forward)], MaxAngularSpeeds[int(ELocalAxis::Forward)]);
	float& yawSpeed = AngularSpeeds[int(ELocalAxis::Up)];
	yawSpeed += AngularAccelerations[int(ELocalAxis::Up)] * dt;
	if (yawSpeed <= 0.f) yawSpeed = std::clamp(yawSpeed, -MaxAngularSpeeds[int(ELocalAxis::Up)], -MinAngularSpeeds[int(ELocalAxis::Up)]);
	else yawSpeed = std::clamp(yawSpeed, MinAngularSpeeds[int(ELocalAxis::Up)], MaxAngularSpeeds[int(ELocalAxis::Up)]);
}

void CArwing::TurnRight(float const dt)
{
	float& rollSpeed = AngularSpeeds[int(ELocalAxis::Forward)];
	rollSpeed += AngularAccelerations[int(ELocalAxis::Forward)] * dt;
	if (rollSpeed <= 0.f) rollSpeed = std::clamp(rollSpeed, -MaxAngularSpeeds[int(ELocalAxis::Forward)], -MinAngularSpeeds[int(ELocalAxis::Forward)]);
	else rollSpeed = std::clamp(rollSpeed, MinAngularSpeeds[int(ELocalAxis::Forward)], MaxAngularSpeeds[int(ELocalAxis::Forward)]);
	float& yawSpeed = AngularSpeeds[int(ELocalAxis::Up)];
	yawSpeed -= AngularAccelerations[int(ELocalAxis::Up)] * dt;
	if (yawSpeed <= 0.f) yawSpeed = std::clamp(yawSpeed, -MaxAngularSpeeds[int(ELocalAxis::Up)], -MinAngularSpeeds[int(ELocalAxis::Up)]);
	else yawSpeed = std::clamp(yawSpeed, MinAngularSpeeds[int(ELocalAxis::Up)], MaxAngularSpeeds[int(ELocalAxis::Up)]);
}

void CArwing::GoUp(float const dt)
{
	float& pitchSpeed = AngularSpeeds[int(ELocalAxis::Right)];
	pitchSpeed += AngularAccelerations[int(ELocalAxis::Right)] * dt;
	if (pitchSpeed <= 0.f) pitchSpeed = std::clamp(pitchSpeed, -MaxAngularSpeeds[int(ELocalAxis::Right)], -MinAngularSpeeds[int(ELocalAxis::Right)]);
	else pitchSpeed = std::clamp(pitchSpeed, MinAngularSpeeds[int(ELocalAxis::Right)], MaxAngularSpeeds[int(ELocalAxis::Right)]);
}

void CArwing::GoDown(float const dt)
{
	float& pitchSpeed = AngularSpeeds[int(ELocalAxis::Right)];
	pitchSpeed -= AngularAccelerations[int(ELocalAxis::Right)] * dt;
	if (pitchSpeed <= 0.f) pitchSpeed = std::clamp(pitchSpeed, -MaxAngularSpeeds[int(ELocalAxis::Right)], -MinAngularSpeeds[int(ELocalAxis::Right)]);
	else pitchSpeed = std::clamp(pitchSpeed, MinAngularSpeeds[int(ELocalAxis::Right)], MaxAngularSpeeds[int(ELocalAxis::Right)]);
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

glm::vec3 CArwing::GetPosition() const
{
	return ModelMatrix[3];
}

CCameraTarget CArwing::GetCameraTarget() const
{
	return CCameraTarget(GetPosition(), GetForwardAxis(), GetUpAxis());
}

void CArwing::NormalizeModelMatrix()
{
	glm::vec4& x = ModelMatrix[0];
	glm::vec4& y = ModelMatrix[1];
	glm::vec4& z = ModelMatrix[2];
	// Meh... float exact comparision...
	assert(x.w == 0.f && y.w == 0.f && z.w == 0.f);
	x = glm::normalize(x);
	y = glm::normalize(y);
	z = glm::normalize(z);
}