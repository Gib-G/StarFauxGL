#include "Util.h"
#include "Types.h"

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