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
	if (VAO != 0) {
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
	}
}

Mesh::Mesh(Mesh&& other) noexcept
{
	// Steal resources
	vertices = std::move(other.vertices);
	indices = std::move(other.indices);
	VAO = other.VAO; other.VAO = 0;
	VBO = other.VBO; other.VBO = 0;
	EBO = other.EBO; other.EBO = 0;
	_path = std::move(other._path);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other) {
		// free existing resources
		if (VAO != 0) glDeleteVertexArrays(1, &VAO);
		if (VBO != 0) glDeleteBuffers(1, &VBO);
		if (EBO != 0) glDeleteBuffers(1, &EBO);

		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
		VAO = other.VAO; other.VAO = 0;
		VBO = other.VBO; other.VBO = 0;
		EBO = other.EBO; other.EBO = 0;
		_path = std::move(other._path);
	}
	return *this;
}

void Mesh::Draw() const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	// create buffers/arrays
	if (VAO == 0) glGenVertexArrays(1, &VAO);
	if (VBO == 0) glGenBuffers(1, &VBO);
	if (EBO == 0) glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (!vertices.empty())
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	if (!indices.empty())
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	else
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	// set the vertex attribute pointers using explicit layout
	auto attributes = GetVertexAttributes();
	for (const auto& attr : attributes) {
		glEnableVertexAttribArray(attr.index);
		glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, sizeof(Vertex), reinterpret_cast<const void*>(attr.offset));
	}

	glBindVertexArray(0);
}

void Mesh::ProcessMesh()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, 0);

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
	std::vector<unsigned int> indices;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
    
	this->vertices = vertices;
	this->indices = indices;
}
