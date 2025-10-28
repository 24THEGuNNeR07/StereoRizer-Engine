#pragma once
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <graphics/RenderObject.h>
#include <graphics/Mesh.h>
#include <graphics/Texture.h>

namespace stereorizer::graphics
{
	class AssimpModel
	{
	public:
		std::vector<Mesh*> meshes;
		std::vector<Material*> materials;
		//std::vector<Texture2D*> texturesLoaded;
		RenderObjectData data;
		std::string directory;
		const Material& material;

		AssimpModel(const char* path, const Material& defMaterial);
		AssimpModel(const AssimpModel& other) = delete; // Prevent copying due to resource management

	private:
		//std::vector<Mesh*> meshes;
		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
		void processMaterials(const aiScene* scene);
		//void loadMaterialProperties(aiMaterial* aiMat, Material* genMat);
		//Texture2D* loadMaterialTextureType(aiMaterial* aiMat, Material* genMat, aiTextureType type, const char* materialName);
	};
}