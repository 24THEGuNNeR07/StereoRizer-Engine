#pragma once
#include <glad.h>

#include <graphics/GLObject.h>

namespace stereorizer::graphics
{
	// Forward declarations
	class VertexBuffer;
	class ElementBuffer;

	enum class DrawType
	{
		TRIANGLES = GL_TRIANGLES, LINES = GL_LINES, POINTS = GL_POINTS
	};
	class VertexArray : public GLObject
	{
	public:

		VertexArray();

		void drawArray(const VertexBuffer& vertexBuffer, DrawType drawType);
		void drawElements(const ElementBuffer& elementBuffer, DrawType drawType);
		GLOBJ_OVERRIDE(VertexArray)
	};
}