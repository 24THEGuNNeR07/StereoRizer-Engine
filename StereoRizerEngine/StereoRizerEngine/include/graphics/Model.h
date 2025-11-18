#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace stereorizer::graphics
{
	class Model {
	public:
		Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader);
		~Model();

		// Copy constructor and assignment
		Model(const Model& other);
		Model& operator=(const Model& other);

		Model(Model&& other) noexcept;
		Model& operator=(Model&& other) noexcept;

		std::shared_ptr<Mesh> GetMesh() const noexcept { return _mesh; }
		std::shared_ptr<Shader> GetShader() const noexcept { return _shader; }
		void SetShader(std::shared_ptr<Shader> shader) noexcept;
		const glm::mat4& GetTransformMatrix() const noexcept { return _transform; }
		void Draw() const;

		// Transformations
		void Translate(const glm::vec3& offset);
		void Rotate(float angle, const glm::vec3& axis);
		void Scale(const glm::vec3& scale);
		void SetColor(const glm::vec3& color) { _color = color; }

	private:
		std::shared_ptr<Mesh> _mesh;
		std::shared_ptr<Shader> _shader;
		glm::mat4 _transform = glm::mat4(1.0f);
		glm::vec3 _color = glm::vec3(1.0f);
	};
}