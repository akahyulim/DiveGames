#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>

namespace Dive
{
	struct ModelParams
	{
		std::string name;
		std::string filepath;
		const aiScene* pAiScene = nullptr;
		//Model* pModel = nullptr;
	};

	class ModelImporter
	{
	public:
		ModelImporter();
		~ModelImporter();

		//bool Load(Model* pModel, const std::string& filepath);

	private:
		//void parseNode(const aiNode* pAiNode, const ModelParams& params, GameObject* pGameObject, GameObject* pParent = nullptr);
		//void parseNodeMeshes(const aiNode* pAiNode, const ModelParams& params, GameObject* pGameObject);
		//MeshRenderable* loadMesh(const ModelParams& params, const aiMesh* pAiMesh, GameObject* pGameObject);
		//void loadMaterial(const ModelParams& params, const aiMesh* pAiMesh, MeshRenderable* pMeshRenderable);

	private:
		//Scene* m_pScene = nullptr;
	};
}