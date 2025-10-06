#pragma once
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Model
{
public:
	Model(std::string meshPath, std::string shaderPath);
	~Model();
	void Draw();
	Shader& GetShader() { return _shader; }

private:
	Mesh _mesh;
	Shader _shader;
	glm::mat4 _transform;
};

