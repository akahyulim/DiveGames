#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>

struct OutModel
{
	std::string outName;
	aiNode* pRootNode = nullptr;
	aiNode* pRootBone = nullptr;
	std::vector<unsigned int> meshIndices;
	std::vector<aiMesh*> meshes;
	std::vector<aiNode*> meshNodes;
	std::vector<aiNode*> bones;
	std::vector<aiAnimation*> animations;
	uint32_t totalNumVertices = 0;
	uint32_t totalNumIndices = 0;
};

class DvModelImporter
{
public:

	bool LoadFromFile(const std::string& filepath);

private:
	void exportModel(aiNode* pRootNode, const std::string& outName);
	void collectMeshes(OutModel& model, aiNode* pNode);
	void collectBones(OutModel& model);
	void buildAndSaveModel(OutModel& model);

private:
	// model
	// material
	// animation
	aiScene* m_pScene = nullptr;

	bool m_bAnimations = false;
	bool m_bMaterials = false;
};