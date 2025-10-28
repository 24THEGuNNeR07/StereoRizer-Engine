#include <graphics/ElementBuffer.h>

using namespace stereorizer::graphics;

ElementBuffer::ElementBuffer(const std::vector<uint32_t>& indices, BufferAccessType accessType, BufferCallType callType) : indicesSize(indices.size())
{
	glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STREAM_DRAW + (int32_t)accessType + (int32_t)callType);
}

ElementBuffer::~ElementBuffer()
{
	glDeleteBuffers(1, &id);
}
