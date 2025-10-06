#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
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
private:
	Camera _camera = Camera(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
};

