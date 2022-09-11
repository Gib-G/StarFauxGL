#pragma once
#include "Types.h"
#include "Model.h"
#include <reactphysics3d/reactphysics3d.h> 

// Generic game entity class (entity-component stuff).
class CEntity
{
public:
	CEntity(std::string const& PathToModelFile); // Load model upon creation.
	CEntity(); // Do not load model upon creation.

	virtual void Update(float const Dt) = 0;
	void Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor, bool const ForceAmbient = false);
	void LoadModel(std::string const& Path);

protected:
	int Hp = 500;
	CModel Model;
	glm::mat4 ModelMatrix = glm::mat4(1.f);
	// Resource managed by rp3d::PhysicsCommon. Do not call delete on this pointer!!
	rp3d::RigidBody* RigidBody = nullptr;

	// Rendering stuff.
	float const ScaleFactor = 1.f; // For uniform scaling of the 3D model.
	void ResetScale(); // Normalizes the model matrix's orientation vectors.
	// For the model to blink when rendered.
	bool IsBlinking = false;
	float BlinkingFrequency = 3.f; // In Hz.
	float BlinkDuration = 0.1f; // In s.

	// Internal time tracker.
	float _Time = 0.f;
};