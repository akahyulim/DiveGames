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
	class Animation;
	struct VertexSkinned;
	class GameObject;
	struct NodeInfo;

	class AssetImporter
	{
	public:
		AssetImporter();
		~AssetImporter();

		bool LoadFromFile(const std::string& filePath);

		// 차리리 LoadFromFile에 참조로 전달하던가 리턴하는 것이 나을 것 같다.
		Model* GetModel() const { return m_pModel; }
		Animation* GetAnimation() const { return m_pAnimation; }

	private:
		void extractBoneWeightForVertices(std::vector<VertexSkinned>& vertices, aiMesh* pMesh, const aiScene* pScene);
		void setVertexBoneData(VertexSkinned& vertex, int boneID, float weight);

		void processNode(const aiScene* pScene, aiNode* pNode, GameObject* pParentGameObject = nullptr);
		void processMeshes(const aiScene* pScene, aiNode* pNode, GameObject* pNodeGameObject);

		void processAnimation(aiAnimation* pAnimation);

		void parseNodes(const aiScene* pScene, aiNode* pNode, NodeInfo* pNodeInfo);
		void parseMeshes(const aiScene* pScene, aiNode* pNode);


	private:
		std::string m_FilePath;

		int m_BoneCounter;

		Model* m_pModel;
		Animation* m_pAnimation;
	};
}