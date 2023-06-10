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
	class Material;
	class GameObject;

	struct AssimpNodeData
	{
		DirectX::XMFLOAT4X4 transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	// 이름 그대로 Importer다. 즉, 외부 파일을 읽어 사용할 수 있는 오브젝트를 구성하는 기능을 가진다.
	// 그리고 추후 AssetExporter를 만들어야 한다.
	class AssetImporter
	{
	public:
		AssetImporter();
		~AssetImporter() = default;

		bool Load(Model* pModel, const std::string& filePath);

	private:
		void parseNode(const aiNode* pNode, GameObject* pParentGameObject = nullptr);
		void parseNodeMeshes(const aiNode* pNode, GameObject* pNodeGameObject);
		void parseMesh(aiMesh* pAiMesh, GameObject* pMeshGameObject);
		Material* parseMaterial(aiMesh* pAiMesh);
		
		void collectBones(aiMesh* pMesh);

		void buildStaticMesh(aiMesh* pMesh, GameObject* pMeshGameObject);
		void buildSkinnedMesh(aiMesh* pMesh, GameObject* pMeshGameObject);

	private:
		std::string m_FilePath;
		std::string m_Name;

		const aiScene* m_pAiScene;

		Model* m_pModel;

		bool m_bAnimation;
	};
}