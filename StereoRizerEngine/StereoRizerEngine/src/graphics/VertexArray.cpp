#include <graphics/VertexArray.h>

using namespace stereorizer::graphics;

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &id);
}

void VertexArray::drawArray(const VertexBuffer& vertexBuffer, DrawType drawType)
{
	//useIfNecessary();
	glDrawArrays((int32_t)drawType, 0, vertexBuffer.vertexCount);
}

void VertexArray::drawElements(const ElementBuffer& elementBuffer, DrawType drawType)
{
	//useIfNecessary();
	glDrawElements((int32_t)drawType, elementBuffer.indicesSize, GL_UNSIGNED_INT, 0);
}