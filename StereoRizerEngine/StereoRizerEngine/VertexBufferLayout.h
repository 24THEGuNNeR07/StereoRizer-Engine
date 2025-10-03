#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:         return 4;
		case GL_UNSIGNED_INT:  return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}
		return 0; // Unsupported type
	}
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> _elements;
    unsigned int _stride = 0;

public:
	VertexBufferLayout()
		: _stride(0) {}
	~VertexBufferLayout() = default;

	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false, "Unsupported type for VertexBufferLayout::Push");
	}

	template<>
	void Push<float>(unsigned int count)
	{
		_elements.push_back({ count, GL_FLOAT, GL_FALSE });
		_stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		_elements.push_back({ count, GL_UNSIGNED_INT, GL_FALSE });
		_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		_elements.push_back({ count, GL_UNSIGNED_BYTE, GL_TRUE });
		_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

    inline const std::vector<VertexBufferElement>& GetElements() const { return _elements; }
    inline unsigned int GetStride() const { return _stride; }
};
