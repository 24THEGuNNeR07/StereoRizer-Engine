#include "Window.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

int main()
{
	//Window window(800, 600, "StereoRizer Engine");

	//window.Run();
	std::string const path = "../models/Cube.obj";
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return -1;
	}
	else {
		std::cout << "Loaded model successfully!" << std::endl;
		std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
	}

    return 0;
}