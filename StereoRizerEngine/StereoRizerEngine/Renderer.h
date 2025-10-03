#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	void Draw(const VertexArray& va, const IndexBuffer& ib, Shader& shader);
	void Clear();
};

