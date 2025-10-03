#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Mesh.h"

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	void Draw(Mesh& mesh, Shader& shader);
	void Clear();
};

