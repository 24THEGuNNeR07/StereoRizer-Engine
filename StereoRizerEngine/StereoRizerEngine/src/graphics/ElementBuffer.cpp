#include <graphics/ElementBuffer.h>

using namespace stereorizer::graphics;

ElementBuffer::ElementBuffer(const std::vector<uint32_t> &indices, BufferAccessType accessType, BufferCallType callType) : indicesSize(indices.size())
{
	glGenBuffers(1, &id);
	use();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STREAM_DRAW + (int32_t)accessType + (int32_t)callType);
}

GLOBJ_DEFAULTS_MULTI_SPEC(ElementBuffer, glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, glDeleteBuffers)

GLOBJ_LAST_USED(ElementBuffer)