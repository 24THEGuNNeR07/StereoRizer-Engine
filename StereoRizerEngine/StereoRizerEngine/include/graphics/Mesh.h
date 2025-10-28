#pragma once
#include <graphics/Vertex.h>
#include <graphics/RenderObject.h>

namespace stereorizer::graphics
{
	class Mesh : public RenderObject
	{
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;


	public:
		Mesh(uint32_t id, RenderObjectData objectParams, Material* const material, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool move);

		virtual void initObject() override;
	};
}