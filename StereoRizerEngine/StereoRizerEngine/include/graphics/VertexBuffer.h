#pragma once
#include <glad.h>
#include <vector>
#include <iostream>

#include <graphics/GLObject.h>
#include <graphics/VertexArray.h>

namespace stereorizer::graphics
{
	enum class BufferAccessType
	{
		STREAM = 0x0, STATIC = 0x4, DYNAMIC = 0x8

	};

	enum class BufferCallType
	{
		DRAW = 0, READ = 1, COPY = 2

	};

	class VertexArray;

	class VertexBuffer : public GLObject
	{
	public:
		const uint32_t vertexCount;

		VertexBuffer(VertexArray& vertexArray, const std::vector<float>& vertices, const std::vector<uint32_t>& attributeSizes, BufferAccessType accessType, BufferCallType callType);

		/*void addVertex(const std::vector<float> &vertexAttributes);

		void loadVertices(BufferAccessType accessType, BufferCallType callType);*/

		//offset of a attribute(not in bytes)
		static const uint32_t attributeOffset(const std::vector<uint32_t>& attributeSizes, uint32_t index);
		//total size of attributes(not in bytes)
		static const uint32_t stride(const std::vector<uint32_t>& attributeSizes);
		GLOBJ_OVERRIDE(VertexBuffer)
	};
}