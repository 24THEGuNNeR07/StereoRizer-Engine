#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"

class Renderer {
public:
	Renderer();
	~Renderer();

	void SetCamera(const Camera& camera); // Accepts external camera
	Camera& GetCamera();                  // Access internal camera

	void Clear();
	void Draw(const Model& model);

private:
	Camera _camera;
};