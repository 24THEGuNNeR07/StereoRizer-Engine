#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Model {
public:
	Model(Mesh mesh, Shader shader);
	~Model();

	Model(Model&& other) noexcept;
	Model& operator=(Model&& other) noexcept;

	const Mesh& GetMesh() const noexcept { return _mesh; }
	const Shader& GetShader() const noexcept { return _shader; }
	void Draw() const;

	// Transformations
	void Translate(const glm::vec3& offset);
	void Rotate(float angle, const glm::vec3& axis);
	void Scale(const glm::vec3& scale);

private:
	Mesh _mesh;
	Shader _shader;
	glm::mat4 _transform = glm::mat4(1.0f);
};

