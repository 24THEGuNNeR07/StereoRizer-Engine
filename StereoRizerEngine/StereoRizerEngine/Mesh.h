#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    
	Mesh(std::string const path);
    void Draw();

private:
	unsigned int VBO, EBO;
    std::string _path;
	void SetupMesh();
	void ProcessMesh();
    void ProcessMeshInternally(aiMesh* mesh, const aiScene* scene);
};

