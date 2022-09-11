#pragma once

// Produces normalized values between 0 and 1.
class CEasingFunction
{
public:
	CEasingFunction(float const Duration);

	// Returns the value of the function at current Time, then Time += Dt.
	virtual float Step(float const Dt) = 0;
	// Returns the value of the function at current Time.
	virtual float GetValue() const = 0;

	void Reset();
	float ResetAndStep(float const Dt);

	bool IsFinished() const;

protected:
	// In seconds.
	float const Duration = 1.f;
	float Time = 0.f;
};

class CEaseInOutExpo : public CEasingFunction
{
	using CParent = CEasingFunction;
public:
	CEaseInOutExpo(float const Duration);

	float Step(float const Dt) override;
	float GetValue() const override;
};