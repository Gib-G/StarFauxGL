#include "Entity.h"
#include "World.h"
#include "Util.h"

CEntity::CEntity(CWorld* const World, EEntityType const Type, CModel* const Model) :
	World(World),
	Type(Type),
	Model(Model)
{
	assert(World);
	assert(Type != EEntityType::EnumCount);
	NormalizingScalingFactor = 1.f;
	if (!Model || !Model->IsLoaded()) return;

	SAABB const& AABB = Model->GetAABB();
	float const originalSize = AABB.GetMaxLength();
	assert(originalSize > 0.f);
	NormalizingScalingFactor = 1.f / originalSize;
}

void CEntity::SetModel(CModel* const Model)
{
	this->Model = Model; if (!Model) return;
	NormalizingScalingFactor = 1.f / Model->GetAABB().GetMaxLength();
}
void CEntity::InitializeRigidBody(rp3d::PhysicsCommon&, rp3d::PhysicsWorld* const) { RigidBody = nullptr; }

void CEntity::DestroyRigidBody(rp3d::PhysicsWorld* const PhysicsWorld)
{
	if (RigidBody) PhysicsWorld->destroyRigidBody(RigidBody);
	RigidBody = nullptr;
}

EEntityType CEntity::GetType() const { return Type; }
glm::vec3 CEntity::GetPosition() const { return glm::vec3(ModelMatrix[3]); }

void CEntity::Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor)
{
	if (!Active) return;
	if (!Model) return;

	ResetScale();
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(NormalizingScalingFactor * Size));
	Model->Draw(CameraPosition, ModelMatrix, ViewMatrix, ProjectionMatrix, LightPosition, LightColor, !DrawTextures);
	ResetScale();
}

void CEntity::SetActive(bool const IsActive)
{
	Active = IsActive; if (RigidBody) RigidBody->setIsActive(IsActive);
}

bool CEntity::IsActive() const { return Active; }

void CEntity::ResetScale()
{
	glm::vec4& x = ModelMatrix[0];
	glm::vec4& y = ModelMatrix[1];
	glm::vec4& z = ModelMatrix[2];
	// Meh... float exact comparision...
	assert(x.w == 0.f && y.w == 0.f && z.w == 0.f);
	
	float const xLength = glm::length(x);
	float const yLength = glm::length(y);
	float const zLength = glm::length(z);
	assert(xLength >= 0.f && yLength >= 0.f && zLength >= 0.f);

	x /= xLength; y /= yLength; z /= zLength;
}

void CEntity::UpdateModelMatrixFromRigidBody(float const InterpolationFactor)
{
	using namespace rp3d;
	if (!RigidBody) return;
	Transform newTransform = Transform::interpolateTransforms(RigidBody->getTransform(), PreviousTransform, InterpolationFactor);
	newTransform.getOpenGLMatrix(reinterpret_cast<decimal*>(&ModelMatrix));
	PreviousTransform = newTransform;

	// The returned matrix shouldn't have any scaling info.
	glm::vec4 x = ModelMatrix[0];
	glm::vec4 y = ModelMatrix[1];
	glm::vec4 z = ModelMatrix[2];
	assert(x.w == 0.f && y.w == 0.f && z.w == 0.f);
	// assert(glm::length(x) == 1.f && glm::length(y) == 1.f && glm::length(z) == 1.f);
}

CEntity& CEntity::operator=(CEntity const& Other)
{
	if (this == &Other) return *this;
	if (World != Other.World)
	{
		assert(false);
		std::cout << "CEntity& operator=(CEntity const&): *this and Other do not belong to the same game world (CWorld instance): Aborting copy!\n";
		return *this;
	}
	if (Type != Other.Type)
	{
		assert(false);
		std::cout
			<< "CEntity& operator=(CEntity const&): *this and Other do not have the same type (EEntityType): Aborting copy!\n"
			<< "Type of *this is: " << s_EntityNames[int(this->GetType())] << ".\n"
			<< "Type of *this is: " << s_EntityNames[int(this->GetType())] << ".\n";
		return *this;
	}
	Active = Other.Active;
	PreviousTransform = Other.PreviousTransform;
	NormalizingScalingFactor = Other.NormalizingScalingFactor;
	Hp = Other.Hp;
	Model = Other.Model;
	ModelMatrix = Other.ModelMatrix;
	// RigidBody = Other.RigidBody; oof noooo don't do this!!
	Size = Other.Size;
	DrawTextures = Other.DrawTextures;

	World->InitializeRigidBody(*this);

	return *this;
}
//////////////////////			ASTEROIDS			///////////////////////////////

CAsteroid::CAsteroid(CWorld* const World, CModel* Model) : CEntity(World, EEntityType::Asteroid, Model)
{
	DrawTextures = false; // If the model used for asteroids is the basic cube with no textures.
	Randomize();
}

// !! Call Randomize before this to get the right size! !!
void CAsteroid::InitializeRigidBody(rp3d::PhysicsCommon& PhysicsCommon, rp3d::PhysicsWorld* const PhysicsWorld)
{
	using namespace rp3d;
	Transform transform; transform.setFromOpenGL(reinterpret_cast<decimal*>(&ModelMatrix));
	RigidBody = PhysicsWorld->createRigidBody(transform);

	SphereShape* const sphere = PhysicsCommon.createSphereShape(Size);
	RigidBody->addCollider(sphere, Transform::identity());

	RigidBody->setLinearDamping(0.f);
	RigidBody->setAngularDamping(0.f);
	RigidBody->setMass(Mass);

	RigidBody->setUserData(this);
}

void CAsteroid::Update(float const Dt)
{
	// Despawn.
	if (glm::length(World->GetArwingPosition() - GetPosition()) >= DespawnDistance) SetActive(false);

	UpdateModelMatrixFromRigidBody(World->GetInterpolationFactor());
}

void CAsteroid::OnCollision(CEntity const& CollidedWith)
{
	EEntityType type = CollidedWith.GetType();
	if (type != EEntityType::Arwing) return;

	CArwing const* arwing = dynamic_cast<CArwing const*>(&CollidedWith);
	assert(arwing);

	// Bon, ça a été fait un peu à l'arrache...
	glm::vec3 const& temp = arwing->GetForwardAxis();
	rp3d::Vector3 const arwingForwardAxis(temp.x, temp.y, temp.z);
	RigidBody->setLinearVelocity(500.f * arwingForwardAxis); // Bumps the hit asteroid forward!
}

void CAsteroid::Randomize(SParams const& Params)
{
	// Some checks.
	assert(Params.MinSize > 0.f && Params.MinSize <= Params.MaxSize);
	assert(Params.MinMass > 0.f && Params.MinMass <= Params.MaxMass);
	assert(0.f <= Params.MinAngularVelocity && Params.MinAngularVelocity <= Params.MaxAngularVelocity);
	assert(0.f <= Params.MinLinearVelocity && Params.MinLinearVelocity <= Params.MaxLinearVelocity);
	assert(0.f <= Params.MinSpawnDistanceFromPlayer && Params.MinSpawnDistanceFromPlayer <= Params.MaxSpawnDistanceFromPlayer);

	CRandomizer r;

	// Randomizes initial position
	ModelMatrix = glm::mat4(1.f);
	ModelMatrix = glm::translate(ModelMatrix, Params.PlayerPosition);

	glm::vec3 randomDirection; r.GetRandomVector(randomDirection);
	float const randomDistance = r.GetRandomFloat(Params.MinSpawnDistanceFromPlayer, Params.MaxSpawnDistanceFromPlayer);
	ModelMatrix = glm::translate(ModelMatrix, randomDistance * randomDirection);

	// Randomizes initial transform.
	glm::vec3 randomRotationAxis; r.GetRandomVector(randomRotationAxis);
	float const randomRotationAngle = glm::radians(r.GetRandomFloat(0.f, 360.f));
	ModelMatrix = glm::rotate(ModelMatrix, randomRotationAngle, randomRotationAxis);

	// Randomizes size and mass.
	Size = r.GetRandomFloat(Params.MinSize, Params.MaxSize);
	Mass = r.GetSameRandomFloat(Params.MinMass, Params.MaxMass);

	// Randomizes velocities.
	r.GetRandomVector(LinearVelocity);
	LinearVelocity *= r.GetRandomFloat(Params.MinLinearVelocity, Params.MaxLinearVelocity);
	
	r.GetRandomVector(AngularVelocity);
	AngularVelocity *= r.GetRandomFloat(Params.MinAngularVelocity, Params.MaxAngularVelocity);

	// Applying transforms to the rigid body.
	if (!RigidBody) return;
	rp3d::Transform transform; transform.setFromOpenGL(reinterpret_cast<rp3d::decimal*>(&ModelMatrix));
	RigidBody->setTransform(transform);

	RigidBody->setLinearVelocity(LinearVelocity);
	RigidBody->setAngularVelocity(AngularVelocity);

	RigidBody->setMass(Mass);
}