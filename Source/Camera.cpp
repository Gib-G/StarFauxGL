#include "Camera.h"

CCameraTarget::CCameraTarget(glm::vec3 const& Position, glm::vec3 const& ForwardAxis, glm::vec3 const& UpAxis)
{
	this->Position = Position;
	this->ForwardAxis = glm::normalize(ForwardAxis);
	this->UpAxis = glm::normalize(UpAxis);
};

glm::vec3 CCameraTarget::GetPosition() const { return Position; }
glm::vec3 CCameraTarget::GetForwardAxis() const { return ForwardAxis; }
glm::vec3 CCameraTarget::GetUpAxis() const { return UpAxis; }
