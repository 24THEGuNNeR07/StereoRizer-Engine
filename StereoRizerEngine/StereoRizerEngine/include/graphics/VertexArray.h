#pragma once
#include <glad/glad.h>
#include <graphics/VertexBuffer.h>
#include <graphics/ElementBuffer.h>

namespace stereorizer::graphics
{
	enum class DrawType
	{
		TRIANGLES = GL_TRIANGLES, LINES = GL_LINES, POINTS = GL_POINTS
	};
	class VertexArray
	{
	private:
		GLuint id;
	public:

		VertexArray();
		~VertexArray();

		void drawArray(const VertexBuffer& vertexBuffer, DrawType drawType);
		void drawElements(const ElementBuffer& elementBuffer, DrawType drawType);
	};
}
