#pragma once
#include "Dive.h"

#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>

class AssetImporter
{
public:
	AssetImporter();
	~AssetImporter();

	bool LoadFromFile(const std::string& filePath);
	bool SaveToFile(const std::string& filePath);

	void Clear();

	std::string GetFilePath() const { return m_FilePath; }
	Dive::Model* GetModel() const { return m_pModel; }

private:
	bool loadExternalExtension(const std::string& filePath);
	bool loadEngineExtension(const std::string& filePath);

	void parseAndCreateNode(const aiScene* pScene, aiNode* pNode, Dive::GameObject* pNodeGameObject, Dive::Transform* pParent = nullptr);

	void parseAndCreateMeshes(const aiScene* pScene, aiNode* pNode, Dive::GameObject* pGameObject);
	Dive::Material* parseAndCreateMaterials(const aiScene* pScene, aiMesh* pMesh);

private:
	std::string m_FilePath;
	Dive::Model* m_pModel;
};