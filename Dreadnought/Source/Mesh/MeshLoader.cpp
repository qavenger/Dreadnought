#include <stdafx.h>
#include "MeshLoader.h"

// Assimp loder
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

void IMeshLoader::LoadFromFile(std::string fileName)
{
	Assimp::Importer importer;
	auto* scene = importer.ReadFile(
		fileName, 
		aiProcess_CalcTangentSpace 
		| aiProcess_Triangulate 
		| aiProcess_JoinIdenticalVertices
		| aiProcess_SortByPType
		| aiProcess_MakeLeftHanded
	);

	if (!scene)
	{
		return;
	}
}
