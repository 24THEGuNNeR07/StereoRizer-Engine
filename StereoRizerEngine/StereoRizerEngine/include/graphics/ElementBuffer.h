#pragma once
#include <graphics/GLObject.h>
#include <vector>
#include <iostream>
#include <graphics/VertexBuffer.h>

namespace stereorizer::graphics
{
	class ElementBuffer : public GLObject
	{
	public:
		const uint32_t indicesSize;

		ElementBuffer(const std::vector<uint32_t>& indices, BufferAccessType accessType, BufferCallType callType);

		GLOBJ_OVERRIDE(ElementBuffer)
	};
}