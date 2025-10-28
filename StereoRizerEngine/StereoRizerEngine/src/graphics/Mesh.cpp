#include "graphics/Mesh.h"
#include "core/Common.h"

using namespace stereorizer::graphics;

Mesh::Mesh(const std::string& path)
{
	_path = path;
	ProcessMesh();
	this->vertices = vertices;
	this->indices = indices;
	SetupMesh();
}

Mesh::~Mesh()
{
	/*if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
		EBO = 0;
	}*/
}

Mesh::Mesh(Mesh&& other) noexcept
{
	// Steal resources
	vertices = std::move(other.vertices);
	indices = std::move(other.indices);
	/*VAO = other.VAO; other.VAO = 0;
	VBO = other.VBO; other.VBO = 0;
	EBO = other.EBO; other.EBO = 0;*/
	_path = std::move(other._path);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other) {
		// free existing resources
		/*if (VAO != 0) glDeleteVertexArrays(1, &VAO);
		if (VBO != 0) glDeleteBuffers(1, &VBO);
		if (EBO != 0) glDeleteBuffers(1, &EBO);*/

		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
		/*VAO = other.VAO; other.VAO = 0;
		VBO = other.VBO; other.VBO = 0;
		EBO = other.EBO; other.EBO = 0;*/
		_path = std::move(other._path);
	}
	return *this;
}

void Mesh::Draw() const
{
	if (elementBuffer != nullptr)
	{
		vtxArray->drawElements(*elementBuffer, DrawType::TRIANGLES);
		return;
	}
	vtxArray->drawArray(*vtxBuffer, DrawType::TRIANGLES);
}

void Mesh::SetupMesh()
{
	vtxArray = new VertexArray();
	std::vector<uint32_t> attributesSizes = { 3,3 };
	std::vector<float> verticesFloat;
	verticesFloat.reserve(vertices.size() * 6);
	for (auto& vertex : vertices)
		verticesFloat.insert(verticesFloat.end(), (float*)&vertex, (float*)(&vertex + 1));
	vtxBuffer = new VertexBuffer(*vtxArray, verticesFloat, attributesSizes, BufferAccessType::STATIC, BufferCallType::DRAW);
	elementBuffer = new ElementBuffer(indices, BufferAccessType::STATIC, BufferCallType::DRAW);
}

void Mesh::ProcessMesh()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		LOG_ERROR(std::string("ERROR::ASSIMP:: ") + importer.GetErrorString());
		return;
	}
	else {
		LOG_INFO("Loaded model successfully!");
		LOG_INFO(std::string("Number of meshes: ") + std::to_string(scene->mNumMeshes));
	}

	ProcessMeshInternally(scene->mMeshes[0]);
}

void Mesh::ProcessMeshInternally(aiMesh* mesh) 
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		vertices.push_back(vertex);
	}

	indices.reserve((size_t)mesh->mNumFaces * 3);

	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
    
	this->vertices = vertices;
	this->indices = indices;
}
