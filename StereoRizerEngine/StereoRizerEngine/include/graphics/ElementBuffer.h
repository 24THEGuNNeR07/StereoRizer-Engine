#pragma once
#include <vector>
#include <graphics/VertexBuffer.h>
#include <glad/glad.h>

namespace stereorizer::graphics
{
	class ElementBuffer
	{
	private:
		GLuint id;
	public:
		const int indicesSize;
		ElementBuffer(const std::vector<uint32_t>& indices, BufferAccessType accessType, BufferCallType callType);
		~ElementBuffer();
	};
}