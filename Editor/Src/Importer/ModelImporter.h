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
}

class ModelImporter
{
public:
	ModelImporter();
	~ModelImporter();

	bool Load(Dive::Model* pModel, const std::string& filepath);

	void SetScene(Dive::Scene* pScene) { m_pScene = pScene; }

private:
	void parseNode(const aiNode* pNode, const aiScene* pScene, Dive::GameObject* pGameObject, Dive::GameObject* pParent);
	void parseNodeMeshes(const aiNode* pNode, const aiScene* pScene, Dive::GameObject* pGameObject);
	void loadMesh(const aiMesh* pMesh, Dive::GameObject* pGameObject);

private:
	Dive::Scene* m_pScene = nullptr;
	Dive::Model* m_pModel = nullptr;
};