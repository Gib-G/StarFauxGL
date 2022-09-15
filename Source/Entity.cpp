#include "Entity.h"
#include "World.h"
#include <random>
#include <chrono> 

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
	//assert(glm::length(x) == 1.f && glm::length(y) == 1.f && glm::length(z) == 1.f);
}

////////	ASTEROIDS	 //////////
CAsteroid::CAsteroid(CWorld* const World, CModel* Model) : CEntity(World, EEntityType::Asteroid, Model)
{
	DrawTextures = false;
	//ModelMatrix = glm::mat4(1.f);
	//ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.f, 0.f, -200.f));
	//Size = 5.f;
	Randomize();
}

// !! Call Randomize before this to get the right size !!
void CAsteroid::InitializeRigidBody(rp3d::PhysicsCommon& PhysicsCommon, rp3d::PhysicsWorld* const PhysicsWorld)
{
	using namespace rp3d;
	Transform transform; transform.setFromOpenGL(reinterpret_cast<decimal*>(&ModelMatrix));
	RigidBody = PhysicsWorld->createRigidBody(transform);
	SphereShape* const sphere = PhysicsCommon.createSphereShape(Size);
	RigidBody->addCollider(sphere, Transform::identity());

	RigidBody->setLinearDamping(0.f);
	RigidBody->setAngularDamping(0.f);
	RigidBody->setMass(100.f);

	RigidBody->setUserData(this);
}

void CAsteroid::Update(float const Dt)
{
	UpdateModelMatrixFromRigidBody(World->GetInterpolationFactor());
}

void CAsteroid::OnCollision(CEntity const& CollidedWith)
{
	EEntityType type = CollidedWith.GetType();
	if (type != EEntityType::Arwing) return;

	CArwing const* arwing = dynamic_cast<CArwing const*>(&CollidedWith);
	assert(arwing);

	glm::vec3 const& f = arwing->GetForwardAxis();
	rp3d::Vector3 ff(f.x, f.y, f.z);
	RigidBody->setLinearVelocity(500.f * ff);
}

static float Lerp(float const x1, float const x2, float const dx)
{
	assert(0.f <= dx && dx <= 1.f);
	return dx * x2 + (1.f - dx) * x1;
}

void CAsteroid::Randomize(SParams const& Params)
{
	// Some checks.
	assert(Params.MinSize > 0.f && Params.MinSize <= Params.MaxSize);
	assert(0.f <= Params.MinAngularVelocity && Params.MinAngularVelocity <= Params.MaxAngularVelocity);
	assert(0.f <= Params.MinLinearVelocity && Params.MinLinearVelocity <= Params.MaxLinearVelocity);
	assert(0.f <= Params.MinSpawnDistanceFromPlayer && Params.MinSpawnDistanceFromPlayer <= Params.MaxSpawnDistanceFromPlayer);

	// Setting up the Mersenne twister.
	double const u32Max = double(uint32_t(-1));
	std::mt19937 mt(unsigned int(std::chrono::steady_clock::now().time_since_epoch().count()));
	//std::mt19937 mt(std::random_device{}());

	// Randomizes initial position
	ModelMatrix = glm::mat4(1.f);
	ModelMatrix = glm::translate(ModelMatrix, Params.PlayerPosition);
	glm::vec3 randomDirection
	(
		Lerp(-1.f, 1.f, double(mt()) / u32Max),
		Lerp(-1.f, 1.f, double(mt()) / u32Max),
		Lerp(-1.f, 1.f, double(mt()) / u32Max)
	); float length = glm::length(randomDirection);
	randomDirection = (length == 0.f ? glm::vec3(0.f, -1.f, 0.f) : randomDirection / length);
	float const randomDistance = Lerp(Params.MinSpawnDistanceFromPlayer, Params.MaxSpawnDistanceFromPlayer, float(mt() / u32Max));
	ModelMatrix = glm::translate(ModelMatrix, randomDistance * randomDirection);

	// Randomizes initial transform.
	glm::vec3 randomRotationAxis =
	{
		Lerp(-1.f, 1.f, float(mt() / u32Max)),
		Lerp(-1.f, 1.f, float(mt() / u32Max)),
		Lerp(-1.f, 1.f, float(mt() / u32Max))
	};
	length = glm::length(randomRotationAxis);
	randomRotationAxis = (length == 0.f ? glm::vec3(0.f, -1.f, 0.f) : randomRotationAxis);
	float const randomRotationAngle = glm::radians(Lerp(0.f, 360.f, float(mt() / u32Max)));
	ModelMatrix = glm::rotate(ModelMatrix, randomRotationAngle, randomRotationAxis);

	// Randomizes size.
	Size = Lerp(Params.MinSize, Params.MaxSize, float(mt() / u32Max));

	using namespace rp3d;

	// Randomizes velocities.
	LinearVelocity = Vector3
	(
		Lerp(-1.f, 1.f, float(mt() / u32Max)),
		Lerp(-1.f, 1.f, float(mt() / u32Max)),
		Lerp(-1.f, 1.f, float(mt() / u32Max))
	);
	length = LinearVelocity.length();
	LinearVelocity = (length == 0.f ? rp3d::Vector3(1.f, 0.f, 0.f) : LinearVelocity / length);
	float const randomLinearVelocity = Lerp(Params.MinLinearVelocity, Params.MaxLinearVelocity, float(mt() / u32Max));
	LinearVelocity *= randomLinearVelocity;

	AngularVelocity = Vector3
	(
		Lerp(-1.f, 1.f, float(mt() / u32Max)),
		Lerp(-1.f, 1.f, float(mt() / u32Max)),
		Lerp(-1.f, 1.f, float(mt() / u32Max))
	);
	length = AngularVelocity.length();
	AngularVelocity = (length == 0.f ? Vector3(1.f, 0.f, 0.f) : AngularVelocity / length);
	float const randomAngularVelocity = Lerp(Params.MinAngularVelocity, Params.MaxAngularVelocity, float(mt() / u32Max));
	AngularVelocity *= randomAngularVelocity;

	if (!RigidBody) return;
	Transform transform; transform.setFromOpenGL(reinterpret_cast<decimal*>(&ModelMatrix));
	RigidBody->setTransform(transform);
	RigidBody->setLinearVelocity(LinearVelocity);
	RigidBody->setAngularVelocity(AngularVelocity);
}