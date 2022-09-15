#pragma once
#include "Types.h"
#include <random>
#include <chrono>
#include <reactphysics3d/reactphysics3d.h>

// Produces normalized values between 0 and 1.
class CEasingFunction
{
public:
	CEasingFunction(float const Duration);

	// Returns the value of the function at current Time.
	virtual float GetValue() const;
	// Returns the value of the function at current Time, then Time += Dt.
	float Step(float const Dt);

	void Reset();
	float ResetAndStep(float const Dt);

	bool IsFinished() const;

protected:
	// In seconds.
	float const Duration = 1.f;
	// Normalized time (in [0, 1]). No units.
	float Time = 0.f;
};

class CEaseInOutExpo : public CEasingFunction
{
	using CParent = CEasingFunction;
public:
	CEaseInOutExpo(float const Duration);

	float GetValue() const override;
};

class CSquareWave : public CEasingFunction
{
	using CParent = CEasingFunction;
public:
	CSquareWave(float const Duration, uint8_t const NumberOfBlinks);

	float GetValue() const override;

private:
	int const NumberOfBlinks = 1;
	float const SingleBlinkDuration = 0.5f / NumberOfBlinks;
};

class CRandomizer
{
public:
	CRandomizer();

	// Uniform distribution on [Min, Max].
	float GetRandomFloat(float const Min, float const Max);
	// Allows to use the same random number generated via mt() in the previous call to GetRandomFloat.
	// But this time, it can be linearly interpolated from different bounds.
	float GetSameRandomFloat(float const Min, float const Max);

	// Returns non-zero normalized random vectors.
	void GetRandomVector(glm::vec3& VectorOut);
	void GetRandomVector(rp3d::Vector3& VectorOut);

private:
	// Mersenne twister.
	std::mt19937 mt;
	float const u32Max_f = float(uint32_t(-1));

	// Between 0 and 1.
	float LastRandomNumber = -1.f;
};