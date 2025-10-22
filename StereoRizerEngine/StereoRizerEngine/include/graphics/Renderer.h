#pragma once

#include <memory>

#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"


class Renderer {
public:
	Renderer();
	~Renderer();

	std::shared_ptr<Camera> GetCamera();

	void Clear();
	void Draw(std::shared_ptr<Model> model);

private:
	std::shared_ptr<Camera> _camera;
};