#pragma once
#include "Types.h"
#include "Model.h"

class CCameraTarget;

class CArwing
{
public:
	CArwing(bool const LoadModelNow = false);

	void LoadModel();
	void Draw(glm::vec3 const& CameraPosition, glm::mat4 const& View, glm::mat4 const& Projection, glm::vec3 const& LightPosition, glm::vec3 const& LightColor, bool ForceAmbient = false);
	
	// Moves the Arwing (rotations and translations). Needs to be called every frame!
	void Move(float const dt);

	// Linear motion. These only change the linear velocity. Motion is only applied when CArwing::Move is called.
	void Accelerate(float const dt);
	void Deccelerate(float const dt);

	// Angular motion. These only change the angular velocities. Motion is only applied when CArwing::Move is called.
	void TurnLeft(float const dt);
	void TurnRight(float const dt);
	void GoUp(float const dt);
	void GoDown(float const dt);

	// Returns normalized transformed axes.
	// Hardcoded and model-specific: may need to be changed if using an other 3D model.
	glm::vec3 GetForwardAxis() const;
	glm::vec3 GetUpAxis() const;
	glm::vec3 GetRightAxis() const;

	glm::vec3 GetPosition() const;

	// Returns info about the current position and orientation of the Arwing in the SCameraTarget format
	// manipulated by the CCamera class.
	CCameraTarget GetCameraTarget() const;

private:
	// Linear motion params (in SI units, absolute values).
	float const LinearAcceleration = 0.f;
	float const LinearDrag = 5.f;
	float const LinearDecceleration = 30.f;
	float const MaxLinearSpeed = 400.f;
	float const MinLinearSpeed = 0.f;
	float LinearSpeed = MinLinearSpeed;

	enum class ELocalAxis : uint8_t { Forward, Up, Right, EnumCount };
	// Model-specific.
	glm::vec3 const LocalAxes[int(ELocalAxis::EnumCount)] =
	{
		{ 0.f, 1.f, 0.f },
		{ 0.f, 0.f, -1.f },
		{ -1.f, 0.f, 0.f }
	};
	// Angular motion params per local axis (absolute values).
	// Units : deg/s or deg/s^2.
	float const AngularAccelerations[int(ELocalAxis::EnumCount)] = {270.f, 270.f, 270.f};
	float const AngularDeccelerations[int(ELocalAxis::EnumCount)] = {120.f, 120.f, 120.f};
	float const MaxAngularSpeeds[int(ELocalAxis::EnumCount)] = {70.f, 70.f, 70.f};
	float const MinAngularSpeeds[int(ELocalAxis::EnumCount)] = {0.f, 0.f, 0.f};
	float AngularSpeeds[int(ELocalAxis::EnumCount)] = {0.f, 0.f, 0.f};

	CModel Model;
	glm::mat4 ModelMatrix = glm::mat4(1.f);

	// Normalizes ModelMatrix[0, 1, and 2].
	void NormalizeModelMatrix();
};