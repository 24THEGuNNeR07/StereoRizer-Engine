#include <graphics/Mesh.h>

using namespace stereorizer::graphics;

Mesh::Mesh(uint32_t id, RenderObjectData objectParams, Material *const material, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, bool move) : RenderObject(id, objectParams, material)
{
	if(move)
	{
		this->vertices = std::move(vertices);
		this->indices = std::move(indices);
	}
	else
	{
		this->vertices = vertices;
		this->indices = indices;
	}
}


void Mesh::initObject()
{
	vtxArray = new VertexArray();
	std::vector<uint32_t> attributesSizes = {3,3,2};
	std::vector<float> verticesFloat;
	verticesFloat.reserve(vertices.size() * 8);
	for(auto &vertex : vertices)
		verticesFloat.insert(verticesFloat.end(), (float *)&vertex, (float *)(&vertex + 1));
	vtxBuffer = new VertexBuffer(*vtxArray, verticesFloat, attributesSizes, BufferAccessType::STATIC, BufferCallType::DRAW);
	elementBuffer = new ElementBuffer(indices, BufferAccessType::STATIC, BufferCallType::DRAW);
}