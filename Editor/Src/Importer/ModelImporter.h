#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>

namespace Dive
{
	class Model;
	class Scene;
	class GameObject;
	class MeshRenderable;
}

class ModelImporter
{
public:
	ModelImporter();
	~ModelImporter();

	bool Load(Dive::Model* pModel, const std::string& filepath);

	void SetScene(Dive::Scene* pScene) { m_pScene = pScene; }

private:
	void parseNode(const aiNode* pAiNode, const aiScene* pAiScene, Dive::GameObject* pGameObject, Dive::GameObject* pParent);
	void parseNodeMeshes(const aiNode* pAiNode, const aiScene* pAiScene, Dive::GameObject* pGameObject);
	Dive::MeshRenderable* loadMesh(const aiMesh* pAiMesh, Dive::GameObject* pGameObject);
	void loadMaterial(const aiScene* pAiScene, const aiMesh* pAiMesh, Dive::MeshRenderable* pMeshRenderable);

private:
	Dive::Scene* m_pScene = nullptr;
	Dive::Model* m_pModel = nullptr;
};