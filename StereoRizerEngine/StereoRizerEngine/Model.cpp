#include "Model.h"

Model::Model(std::string meshPath, std::string shaderPath) :
	_mesh(meshPath), _shader(shaderPath)
{
	_transform = glm::mat4(1.0f);
}

Model::~Model()
{

}

void Model::Draw()
{
	_shader.Bind();
	_mesh.Draw();
}
