#include <graphics/VertexBuffer.h>
#include <iostream>
#include <graphics/VertexArray.h>

using namespace stereorizer::graphics;

VertexBuffer::VertexBuffer(VertexArray& vertexArray, const std::vector<float>& vertices, const std::vector<uint32_t>& attributeSizes, BufferAccessType accessType, BufferCallType callType) : vertexCount((uint32_t)vertices.size() / stride(attributeSizes))
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STREAM_DRAW + (int32_t)accessType + (int32_t)callType);
	uint32_t strideSize = stride(attributeSizes) * sizeof(float);

	for (uint32_t i = 0; i < attributeSizes.size(); i++)
	{
		glVertexAttribPointer(i, attributeSizes[i], GL_FLOAT, GL_FALSE, strideSize, (void*)(attributeOffset(attributeSizes, i) * sizeof(float)));
		glEnableVertexAttribArray(i);
	}
}

const uint32_t VertexBuffer::attributeOffset(const std::vector<uint32_t>& attributeSizes, uint32_t index)
{
	uint32_t tot = 0;
	for (uint32_t i = 0; i < index && i < attributeSizes.size(); i++)
	{
		tot += attributeSizes[i];
	}
	return tot;
}

const uint32_t VertexBuffer::stride(const std::vector<uint32_t>& attributeSizes)
{
	return attributeOffset(attributeSizes, (uint32_t)attributeSizes.size());
}