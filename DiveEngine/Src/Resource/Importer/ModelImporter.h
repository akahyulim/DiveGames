#pragma once
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>

namespace Dive
{
	class Model;
	class GameObject;

	class ModelImporter
	{
	public:
		ModelImporter();
		~ModelImporter() = default;

		bool Load(Model* pModel, const std::string& filePath);

	private:
		void parseNode(const aiNode* pNode, GameObject* pParentGameObject = nullptr);
		void parseNodeMeshes(const aiNode* pNode, GameObject* pNodeGameObject);
		void parseMesh(aiMesh* pAiMesh, GameObject* pMeshGameObject);

	private:
		std::string m_FilePath;
		std::string m_Name;

		const aiScene* m_pAiScene;

		Model* m_pModel;

		bool m_bAnimation;
	};
}