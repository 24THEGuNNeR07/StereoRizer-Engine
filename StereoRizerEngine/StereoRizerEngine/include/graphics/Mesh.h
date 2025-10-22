
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstddef>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace stereorizer::graphics
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
    };

    struct VertexAttribute {
        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        size_t offset;
    };

    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO = 0;

        Mesh(const std::string& path);
        ~Mesh();

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void Draw() const;

    protected:
        void SetupMesh();

        static inline std::vector<VertexAttribute> GetVertexAttributes() {
            return {
                { 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position) },
                { 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal) }
            };
        }

    private:
        unsigned int VBO = 0, EBO = 0;
        std::string _path;
        void ProcessMesh();
        void ProcessMeshInternally(aiMesh* mesh);
    };
}