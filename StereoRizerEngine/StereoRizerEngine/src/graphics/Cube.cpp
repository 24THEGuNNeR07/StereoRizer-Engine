//#include "Cube.h"
//
//Cube::Cube()
//{
//    glm::vec3 positions[] = {
//        // Front face
//        {-0.5f, -0.5f,  0.5f},
//        { 0.5f, -0.5f,  0.5f},
//        { 0.5f,  0.5f,  0.5f},
//        {-0.5f,  0.5f,  0.5f},
//        // Back face
//        {-0.5f, -0.5f, -0.5f},
//        { 0.5f, -0.5f, -0.5f},
//        { 0.5f,  0.5f, -0.5f},
//        {-0.5f,  0.5f, -0.5f}
//    };
//
//    // Indices for 12 triangles (2 per face)
//    unsigned int faceIndices[] = {
//        // Front
//        0, 1, 2, 2, 3, 0,
//        // Right
//        1, 5, 6, 6, 2, 1,
//        // Back
//        7, 6, 5, 5, 4, 7,
//        // Left
//        4, 0, 3, 3, 7, 4,
//        // Bottom
//        4, 5, 1, 1, 0, 4,
//        // Top
//        3, 2, 6, 6, 7, 3
//    };
//
//    glm::vec3 faceNormals[] = {
//        { 0.0f,  0.0f,  1.0f}, // Front
//        { 1.0f,  0.0f,  0.0f}, // Right
//        { 0.0f,  0.0f, -1.0f}, // Back
//        {-1.0f,  0.0f,  0.0f}, // Left
//        { 0.0f, -1.0f,  0.0f}, // Bottom
//        { 0.0f,  1.0f,  0.0f}  // Top
//    };
//
//    // Build 6 faces * 6 vertices = 36 total
//    for (int f = 0; f < 6; f++)
//    {
//        for (int i = 0; i < 6; i++)
//        {
//            Vertex vertex;
//            unsigned int index = faceIndices[f * 6 + i];
//            vertex.Position = positions[index];
//            vertex.Normal = faceNormals[f];
//            vertices.push_back(vertex);
//            indices.push_back(f * 6 + i);
//        }
//    }
//
//    this->vertices = vertices;
//    this->indices = indices;
//
//    SetupMesh();
//}
