#pragma once
#include <glm/glm.hpp>

namespace stereorizer::graphics
{
	class Transform
	{
	private:
		bool changedModel = true;

		glm::vec3 position = glm::vec3(0.0f), rotation = glm::vec3(0.0f), scale = glm::vec3(1.0f);
	public:

		const glm::vec3& getPosition() const;
		const glm::vec3& getRotation() const;
		const glm::vec3& getScale() const;
		const bool isModelChanged() const;

		void setPosition(const glm::vec3& position);
		void setRotation(const glm::vec3& rotation);
		void setScale(const glm::vec3& scale);
		void setChangedModel(const bool changedModel);
	};
}