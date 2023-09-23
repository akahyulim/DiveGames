#pragma once
#include "Dive.h"

#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>

struct ModelData
{
	std::string name;
	aiNode* pRootNode = nullptr;
	aiNode* pRootBone = nullptr;
	std::vector<aiMesh*> meshes;
	std::vector<aiNode*> meshNodes;
	std::vector<aiNode*> bones;
	std::vector<aiAnimation*> animations;
	uint32_t numVertices;
	uint32_t numIndices;
};

class AssetImporter
{
public:
	AssetImporter() = default;
	~AssetImporter();

	static bool LoadFromFile(const std::string& filePath);
	static bool LoadExternalFile(const std::string& filePath);
	static bool LoadEngineFile(const std::string& filePath);

	static bool SaveToEngineFile(const std::string& filePath);

	static void Clear();

	static std::string GetFilePath();

	static Dive::Model* GetModel();

	// test
	static void CollectAnimations();

private:
	void loadMaterialTextures(aiMaterial* pMat, aiTextureType type, std::string typeName);

};