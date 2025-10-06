#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	void Draw(Model& model);
	void Clear();
};

