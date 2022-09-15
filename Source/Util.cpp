#include "Util.h"
#include "Types.h"
#include <reactphysics3d/reactphysics3d.h>

CEasingFunction::CEasingFunction(float const Duration) : Duration{ Duration }, Time{ 0.f } { assert(Duration > 0.f); }

float CEasingFunction::Step(float const Dt)
{
	float const res = GetValue();
	assert(Duration > 0.f);
	Time += (Dt / Duration);
	return res;
}
// Default function (instantaneous transition).
float CEasingFunction::GetValue() const { return 1.f; }

void CEasingFunction::Reset() { Time = 0.f; }
float CEasingFunction::ResetAndStep(float const Dt) { Reset(); return Step(Dt); }

bool CEasingFunction::IsFinished() const { return Time >= 1.f; }

float CEaseInOutExpo::GetValue() const
{
	if (Time <= 0.f) return 0.f;
	if (Time >= 1.f) return 1.f;
	if (Time < 0.5f) return 0.5f * std::powf(2.f, 20.f * Time - 10.f);
	return 0.5f * (2.f - std::powf(2.f, -20.f * Time + 10.f));
}

CEaseInOutExpo::CEaseInOutExpo(float const Duration) : CEasingFunction(Duration) {}

CSquareWave::CSquareWave(float const Duration, uint8_t const NumberOfBlinks) : CParent(Duration), NumberOfBlinks(NumberOfBlinks)
{
	assert(NumberOfBlinks >= 1);
}

float CSquareWave::GetValue() const
{
	if (Time >= 1.f) return 0.f;
	int const floor = int(std::floorf(Time / SingleBlinkDuration));
	if (floor % 2 == 0) return 1.f;
	return 0.f;
}

CRandomizer::CRandomizer() { mt = std::mt19937(unsigned int(std::chrono::steady_clock::now().time_since_epoch().count())); }

static float Lerp(float const x1, float const x2, float const dx)
{
	assert(0.f <= dx && dx <= 1.f);
	return dx * x2 + (1.f - dx) * x1;
}

// Uniform distribution on [Min, Max].
float CRandomizer::GetRandomFloat(float const Min, float const Max)
{
	LastRandomNumber = float(mt()) / u32Max_f;
	return Lerp(Min, Max, LastRandomNumber);
}
float CRandomizer::GetSameRandomFloat(float const Min, float const Max)
{
	if (LastRandomNumber < 0.f)
	{
		std::cout << "CRandomizer::GetSameRandomFloat: Please call GetRandomFloat at least once before calling this function. Aborting.\n";
		assert(false);
		return -1;
	}
	return Lerp(Min, Max, LastRandomNumber);
}

// Returns non-zero normalized random vectors.
void CRandomizer::GetRandomVector(glm::vec3& VectorOut)
{
	do
	{
		VectorOut = glm::vec3
		(
			GetRandomFloat(-1.f, 1.f),
			GetRandomFloat(-1.f, 1.f),
			GetRandomFloat(-1.f, 1.f)
		);
	} while (glm::length(VectorOut) == 0.f); // Bad luck... :/
	VectorOut = glm::normalize(VectorOut);
}
void CRandomizer::GetRandomVector(rp3d::Vector3& VectorOut)
{
	do
	{
		VectorOut = rp3d::Vector3
		(
			GetRandomFloat(-1.f, 1.f),
			GetRandomFloat(-1.f, 1.f),
			GetRandomFloat(-1.f, 1.f)
		);
	} while (VectorOut.length() == 0.f);
	VectorOut.normalize();
}