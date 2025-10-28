#include <graphics/Transform.h>

using namespace stereorizer::graphics;

const glm::vec3 &Transform::getPosition() const
{
	return position;
}
const glm::vec3 &Transform::getRotation() const
{
	return rotation;
}
const glm::vec3 &Transform::getScale() const
{
	return scale;
}
const bool Transform::isModelChanged() const
{
	return changedModel;
}

void Transform::setPosition(const glm::vec3 &position)
{
	this->position = position;
	changedModel = true;
}
void Transform::setRotation(const glm::vec3 &rotation)
{
	this->rotation = rotation;
	changedModel = true;
}
void Transform::setScale(const glm::vec3 &scale)
{
	this->scale = scale;
	changedModel = true;
}
void Transform::setChangedModel(const bool changedModel)
{
	this->changedModel = changedModel;
}