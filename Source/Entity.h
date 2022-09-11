#pragma once
#include "Types.h"
#include "Model.h"
#include <reactphysics3d/reactphysics3d.h> 

// Generic game entity class (entity-component stuff).
class CEntity
{
public:

private:
	int Hp = 500;
	glm::mat4 ModelMatrix = glm::mat4(1.f);
	// Resource managed by rp3d::PhysicsCommon. Do not call delete on this pointer!!
	rp3d::RigidBody* RigidBody = nullptr;
};