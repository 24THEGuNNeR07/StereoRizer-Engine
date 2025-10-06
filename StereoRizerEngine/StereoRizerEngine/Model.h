#pragma once
#include "Mesh.h"

class Model
{
public:
	Model(std::string meshPath, std::string shaderPath);
	~Model();
	void Draw();

private:
	Mesh _mesh;
	Shader _shader;
	glm::mat4 _transform;
};

