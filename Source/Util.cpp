#include "Util.h"
#include "Types.h"

CEasingFunction::CEasingFunction(float const Duration) : Duration{ Duration }, Time{ 0.f } { assert(Duration > 0.f); }

void CEasingFunction::Reset() { Time = 0.f; }
float CEasingFunction::ResetAndStep(float const Dt) { Reset(); return Step(Dt); }

bool CEasingFunction::IsFinished() const { return Time > Duration; }

float CEaseInOutExpo::GetValue() const
{
	assert(Duration > 0.f);
	float const normalizedTime = Time / Duration;
	if (normalizedTime <= 0.f) return 0.f;
	if (normalizedTime >= 1.f) return 1.f;
	if (normalizedTime < 0.5f) return 0.5f * std::powf(2.f, 20.f * normalizedTime - 10.f);
	return 0.5f * (2.f - std::powf(2.f, -20.f * normalizedTime + 10.f));
}

CEaseInOutExpo::CEaseInOutExpo(float const Duration) : CEasingFunction(Duration) {};

float CEaseInOutExpo::Step(float const Dt)
{
	float const res = GetValue();
	Time += Dt;
	return res;
}