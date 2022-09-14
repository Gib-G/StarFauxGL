#pragma once
#include "Types.h"
#include "Util.h"

// The axes stored are guaranteed to be normalized!
class CCameraTarget
{
public:
	CCameraTarget() = default;
	CCameraTarget(glm::vec3 const& Position, glm::vec3 const& ForwardAxis, glm::vec3 const& UpAxis);

	glm::vec3 GetPosition() const;
	glm::vec3 GetForwardAxis() const;
	glm::vec3 GetUpAxis() const;

private:
	glm::vec3 Position{ 0.f, 0.f, 0.f };
	glm::vec3 ForwardAxis{ 0.f, 0.f, -1.f };
	glm::vec3 UpAxis{ 0.f, 1.f, 0.f };
};

template<uint8_t TargetBufferSize>
class CCamera
{
public:
	CCamera(CCameraTarget const& InitialTarget, float ForwardAxisOffset, float UpAxisOffset) :
		ForwardAxisOffset{ ForwardAxisOffset },
		UpAxisOffset{ UpAxisOffset }
	{
		static_assert(TargetBufferSize > 0);
		// Prevents the camera from having the exact same location as its target.
		assert(DistanceToLatestTarget > 0.f);
		assert(Index == 0);

		for (CCameraTarget& target : TargetBuffer) { target = InitialTarget; }
		glm::vec3 const& initialTargetForwardAxis = InitialTarget.GetForwardAxis();
		glm::vec3 const& initialTargetUpAxis = InitialTarget.GetUpAxis();

		// Positionning the camera DistanceToLatestTarget behind the initial target.
		Position = InitialTarget.GetPosition() - DistanceToLatestTarget * initialTargetForwardAxis;

		// Applying fixed offsets in the initial target frame.
		Position += ForwardAxisOffset * initialTargetForwardAxis + UpAxisOffset * initialTargetUpAxis;

		ViewMatrix = glm::lookAt(Position, Position + initialTargetForwardAxis, initialTargetUpAxis);
	};

	glm::mat4 GetViewMatrix() const { return ViewMatrix; }
	glm::vec3 GetPosition() const { return Position; }

	void UpdateViewMatrix(CCameraTarget const& LatestTarget)
	{
		// Making sure the camera is DistanceToLatestTarget away from the new target.
		CCameraTarget& currentTarget = TargetBuffer[Index];
		glm::vec3 const& currentTargetPosition = currentTarget.GetPosition();
		glm::vec3 const& currentTargetForwardAxis = currentTarget.GetForwardAxis();
		glm::vec3 const& currentTargetUpAxis = currentTarget.GetUpAxis();
		glm::vec3 const& latestTargetPosition = LatestTarget.GetPosition();
		
		glm::vec3 const currentTargetToLatestTarget = latestTargetPosition - currentTargetPosition;
		assert(glm::dot(currentTargetToLatestTarget, currentTargetForwardAxis) >= 0.f);
		float const currentDistanceToLatestTarget = glm::length(currentTargetToLatestTarget);
		float const distanceToMove = currentDistanceToLatestTarget - DistanceToLatestTarget;

		glm::vec3 const displacementVector = (currentDistanceToLatestTarget == 0.f ? currentTargetForwardAxis : currentTargetToLatestTarget / currentDistanceToLatestTarget);
		Position = currentTargetPosition + distanceToMove * displacementVector;

		// Applying fixed offsets in the current target frame.
		Position += ForwardAxisOffset * currentTargetForwardAxis + UpAxisOffset * currentTargetUpAxis;

		glm::vec3 const lookAt = Position + 0.2f * currentTargetForwardAxis + 0.8f * LatestTarget.GetForwardAxis(); // Adjust the coeffs for the best feel!
		ViewMatrix = glm::lookAt(Position, lookAt, currentTargetUpAxis);

		currentTarget = LatestTarget;
		Index = (Index + 1) % TargetBufferSize;
	}

private:
	// Distance to maintain with target.
	float const DistanceToLatestTarget = 9.f;
	// Fixed offsets in the current camera frame.
	float const ForwardAxisOffset = 0.f;
	float const UpAxisOffset = 0.f;

	glm::mat4 ViewMatrix = glm::mat4(1.f);
	glm::vec3 Position{ 0.f, 0.f, 0.f };

	CCameraTarget TargetBuffer[TargetBufferSize];
	uint8_t Index = 0;
};