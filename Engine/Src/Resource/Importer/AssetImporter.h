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

	// �̸� �״�� Importer��. ��, �ܺ� ������ �о� ����� �� �ִ� ������Ʈ�� �����ϴ� ����� ������.
	// �׸��� ���� AssetExporter�� ������ �Ѵ�.
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