#include "Entity.h"

CEntity::CEntity(std::string const& PathToModelFile) : CEntity()
{
	LoadModel(PathToModelFile);
}

CEntity::CEntity()
{
	assert(BlinkingFrequency >= 0.f);
	assert(BlinkDuration >= 0.f);
}

void CEntity::Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor, bool const ForceAmbient)
{
	ResetScale();
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(ScaleFactor));
	Model.Draw(CameraPosition, ModelMatrix, ViewMatrix, ProjectionMatrix, LightPosition, LightColor, ForceAmbient);
	ResetScale();
}

void CEntity::LoadModel(std::string const& Path)
{
	Model.Load(Path);
}

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