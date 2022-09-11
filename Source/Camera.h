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
	CCamera(CCameraTarget const& InitialTarget, float CurrentTargetForwardAxisOffset, float CurrentTargetUpAxisOffset) :
		CurrentTargetForwardAxisOffset{ CurrentTargetForwardAxisOffset },
		CurrentTargetUpAxisOffset{ CurrentTargetUpAxisOffset }
	{
		static_assert(TargetBufferSize >= 0);
		// Prevents the camera from having the exact same location as its target.
		assert(IdealDistanceToLatestTarget > 0.f);
		assert(ChaseTriggerDistance >= 0.f);
		assert(MinDistanceToLatestTarget >= 0.f);
		assert(MaxDistanceToLatestTarget >= 0.f);
		assert(MinDistanceToLatestTarget <= IdealDistanceToLatestTarget && IdealDistanceToLatestTarget <= MaxDistanceToLatestTarget);
		assert(Index == 0);

		for (CCameraTarget& target : TargetBuffer) { target = InitialTarget; }
		glm::vec3 const& initialTargetForwardAxis = InitialTarget.GetForwardAxis();
		glm::vec3 const& initialTargetUpAxis = InitialTarget.GetUpAxis();

		// Positionning the camera IdealDistanceToLatestTarget behind the initial target.
		Position = InitialTarget.GetPosition() - IdealDistanceToLatestTarget * initialTargetForwardAxis;
		PreviousCompensatedDistance = 0.f;

		// Applying fixed offsets in the initial target frame.
		Position += CurrentTargetForwardAxisOffset * initialTargetForwardAxis + CurrentTargetUpAxisOffset * initialTargetUpAxis;

		ViewMatrix = glm::lookAt(Position, Position + initialTargetForwardAxis, initialTargetUpAxis);
	};
	glm::mat4 GetViewMatrix() const { return ViewMatrix; }
	glm::vec3 GetPosition() const { return Position; }

	void UpdateViewMatrix(CCameraTarget const& LatestTarget, const float Dt)
	{
		CCameraTarget& currentTarget = TargetBuffer[Index];
		glm::vec3 const& currentTargetPosition = currentTarget.GetPosition();
		glm::vec3 const& currentTargetForwardAxis = currentTarget.GetForwardAxis();
		glm::vec3 const& currentTargetUpAxis = currentTarget.GetUpAxis();
		glm::vec3 const& latestTargetPosition = LatestTarget.GetPosition();
		
		glm::vec3 const currentTargetToLatestTarget = latestTargetPosition - currentTargetPosition;
		assert(glm::dot(currentTargetToLatestTarget, currentTargetForwardAxis) >= 0.f);
		float const distanceToLatestTarget = glm::length(currentTargetToLatestTarget);
		//assert(distanceToLatestTarget > 0.f);
		glm::vec3 const displacementVector = (distanceToLatestTarget == 0.f ? currentTargetForwardAxis : currentTargetToLatestTarget / distanceToLatestTarget);
		//glm::vec3 const displacementVector = currentTargetToLatestTarget / distanceToLatestTarget;

		glm::vec3 const positionBeforeCompensation = currentTargetPosition + PreviousCompensatedDistance * displacementVector;
		float const distanceBeforeCompensation = glm::length(latestTargetPosition - positionBeforeCompensation);
		float const distanceToCompensate = distanceBeforeCompensation - IdealDistanceToLatestTarget;

		//if(distanceToCompensate >= ChaseTriggerDistance && ChaseEasingFunction->IsFinished()) ChaseEasingFunction->Reset(); // Might need to test if the previous chase phase is over first...
		float const compensatedDistance = std::clamp
		(
			PreviousCompensatedDistance + distanceToCompensate * ChaseEasingFunction->Step(Dt),
			distanceToLatestTarget - MaxDistanceToLatestTarget,
			distanceToLatestTarget - MinDistanceToLatestTarget
		);
		PreviousCompensatedDistance = compensatedDistance;

		Position = currentTargetPosition + compensatedDistance * displacementVector;

		// Applying fixed offsets in the current target frame.
		Position += CurrentTargetForwardAxisOffset * currentTargetForwardAxis + CurrentTargetUpAxisOffset * currentTargetUpAxis;

		ViewMatrix = glm::lookAt(Position, Position + currentTargetForwardAxis, currentTargetUpAxis);

		currentTarget = LatestTarget;
		Index = (Index + 1) % TargetBufferSize;
	}
	void ChaseTarget() const { if (!ChaseEasingFunction->IsFinished()) return; ChaseEasingFunction->Reset(); }

private:
	std::unique_ptr<CEasingFunction> const ChaseEasingFunction = std::make_unique<CEaseInOutExpo>(2.6f);
	// The following are unsigned distances.
	float const IdealDistanceToLatestTarget = 13.f;
	float const ChaseTriggerDistance = 0.7f;
	float const MinDistanceToLatestTarget = 13.f;
	float const MaxDistanceToLatestTarget = 13.f;
	// Signed distance.
	float PreviousCompensatedDistance = 0.f;

	// Fixed offsets.
	float const CurrentTargetForwardAxisOffset = 0.f;
	float const CurrentTargetUpAxisOffset = 0.f;

	glm::mat4 ViewMatrix = glm::mat4(1.f);
	glm::vec3 Position{ 0.f, 0.f, 0.f };

	CCameraTarget TargetBuffer[TargetBufferSize];
	uint8_t Index = 0;
};