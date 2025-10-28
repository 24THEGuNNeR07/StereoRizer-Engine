#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstddef>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <graphics/Vertex.h>
#include <graphics/VertexArray.h>

namespace stereorizer::graphics
{
    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        //unsigned int VAO = 0;

        Mesh(const std::string& path);
        ~Mesh();

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void Draw() const;

    protected:
        void SetupMesh();

        VertexBuffer* vtxBuffer = nullptr;
        VertexArray* vtxArray = nullptr;
        ElementBuffer* elementBuffer = nullptr;

    private:
        //unsigned int VBO = 0, EBO = 0;
        std::string _path;
        void ProcessMesh();
        void ProcessMeshInternally(aiMesh* mesh);
    };
}