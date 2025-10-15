#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	void Draw(Model& model);
	void Clear();
	void SetCamera(glm::mat4 view, glm::mat4 proj);
private:
	Camera _camera = Camera(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
};

