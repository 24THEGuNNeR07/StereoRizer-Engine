#include <graphics/AssimpModel.h>
#include <graphics/Renderer.h>

using namespace stereorizer::graphics;

AssimpModel::AssimpModel(const char *path, const Material &defMaterial) : material(defMaterial)
{
	loadModel(path);
}

void AssimpModel::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processMaterials(scene);
	processNode(scene->mRootNode, scene);
}



void AssimpModel::processNode(aiNode *node, const aiScene *scene)
{
	// process all the node's meshes (if any)
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* AssimpModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(mesh->mNumVertices);
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		if(mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}
		if(mesh->HasTextureCoords(0))
		{
			vertex.uvs.x = mesh->mTextureCoords[0][i].x;
			vertex.uvs.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.uvs = glm::vec2(0.0f);
		}
		vertices.push_back(vertex);
	}

	indices.reserve((size_t)mesh->mNumFaces * 3);

	for(uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace &face = mesh->mFaces[i];
		for(uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return new Mesh(meshes.size(), this->data, materials[mesh->mMaterialIndex], vertices, indices, true);
}

void AssimpModel::processMaterials(const aiScene *scene)
{
	materials.reserve(scene->mNumMaterials);
	for(uint32_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial *curMat = scene->mMaterials[i];
		Material *genMat = new Material(material, curMat->GetName().C_Str());
		materials.push_back(genMat);

		//loadMaterialProperties(curMat, genMat);

	}
}

void loadColorProperty(aiMaterial *aiMat, Material *genMat, const char *aiKey, uint32_t type, uint32_t idx, const char *matKey)
{
	aiColor3D aiCol;
	aiMat->Get(aiKey, type, idx, aiCol);
	std::cout << matKey << ": r: " << aiCol.r << " g: " << aiCol.g << " b: " << aiCol.b << std::endl;
	glm::vec3 col(aiCol.r, aiCol.g, aiCol.b);
	//genMat->setUniform<glm::vec3>(matKey, col);
}

//void AssimpModel::loadMaterialProperties(aiMaterial *aiMat, Material *genMat)
//{
	//loadColorProperty(aiMat, genMat, AI_MATKEY_COLOR_DIFFUSE, "diffuseCol");
	//loadColorProperty(aiMat, genMat, AI_MATKEY_COLOR_SPECULAR, "specularCol");
	//loadColorProperty(aiMat, genMat, AI_MATKEY_COLOR_AMBIENT, "ambientCol");

	//loadMaterialTextureType(aiMat, genMat, aiTextureType_DIFFUSE, "texture_diffuse");
	//loadMaterialTextureType(aiMat, genMat, aiTextureType_SPECULAR, "texture_specular");
//}

//Texture2D *AssimpModel::loadMaterialTextureType(aiMaterial *aiMat, Material *genMat, aiTextureType type, const char *materialName)
//{
//	Texture2D *texture;
//	aiString str;
//	aiMat->GetTexture(type, 0, &str);
//	if(str.length == 0)
//	{
//		std::cout << "No texture found for " << materialName << " in Material:" << aiMat->GetName().C_Str() << std::endl;
//		return nullptr;
//	}
//	bool skip = false;
//	for(uint32_t j = 0; j < texturesLoaded.size(); j++)
//	{
//		if(std::strcmp(texturesLoaded[j]->path.c_str(), str.C_Str()) == 0)
//		{
//			texture = texturesLoaded[j];
//			skip = true;
//			break;
//		}
//	}
//	if(!skip)
//	{
//		Texture2D *genTexture = new Texture2D((directory + "/" + str.C_Str()).c_str());
//		texture = genTexture;
//		texturesLoaded.push_back(texture);
//	}
//	//genMat->setUniform<Texture2D *>(materialName, texture);
//	return texture;
//}