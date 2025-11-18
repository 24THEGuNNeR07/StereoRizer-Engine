#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"

using namespace stereorizer::graphics;

Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader)
	: _mesh(std::move(mesh)), _shader(std::move(shader))
{
	_transform = glm::mat4(1.0f);
}

Model::~Model() = default;

// Copy constructor
Model::Model(const Model& other)
	: _mesh(other._mesh), _shader(other._shader), _transform(other._transform), _color(other._color) {
	// Shallow copy - shares the same mesh and shader resources
}

// Copy assignment operator
Model& Model::operator=(const Model& other) {
	if (this != &other) {
		_mesh = other._mesh;
		_shader = other._shader;
		_transform = other._transform;
		_color = other._color;
	}
	return *this;
}

Model::Model(Model&& other) noexcept
	: _mesh(std::move(other._mesh)), _shader(std::move(other._shader)), _transform(other._transform), _color(other._color) {}

Model& Model::operator=(Model&& other) noexcept
{
	if (this != &other) {
		_mesh = std::move(other._mesh);
		_shader = std::move(other._shader);
		_transform = other._transform;
	}
	return *this;
}

void stereorizer::graphics::Model::SetShader(std::shared_ptr<Shader> shader) noexcept
{
	_shader = std::move(shader);
}

void Model::Draw() const
{
	GLint modelLoc = glGetUniformLocation(_shader->GetID(), "modelMatrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_transform));

	GLint colorLoc = glGetUniformLocation(_shader->GetID(), "materialColor");
	if (colorLoc != -1)
		glUniform3fv(colorLoc, 1, glm::value_ptr(_color));

	_mesh->Draw();
}

void stereorizer::graphics::Model::BindShader() const
{
	_shader->ReloadIfChanged();
	_shader->Bind();
}

// --- Transformations ---

void Model::Translate(const glm::vec3& offset) {
	_transform = glm::translate(_transform, offset);
}

void Model::Rotate(float angle, const glm::vec3& axis) {
	_transform = glm::rotate(_transform, glm::radians(angle), axis);
}

void Model::Scale(const glm::vec3& scale) {
	_transform = glm::scale(_transform, scale);
}
