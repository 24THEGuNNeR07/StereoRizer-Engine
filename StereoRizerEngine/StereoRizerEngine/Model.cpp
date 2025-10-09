#include "Model.h"

Model::Model(std::string meshPath, std::string shaderPath) :
	_mesh(meshPath), _shader(shaderPath)
{
	_transform = glm::mat4(1.0f);
	_transform = glm::translate(_transform, glm::vec3(0.0f, 0.0f, -5.0f));
	_transform = glm::rotate(_transform, glm::radians(-45.0f), glm::vec3(0.5f, 0.5f, 0.0f));
}

Model::~Model()
{

}

void Model::Draw()
{
	unsigned int transformLoc = glGetUniformLocation(_shader.GetID(), "modelMatrix");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(_transform));
	_shader.Bind();
	_mesh.Draw();
}
