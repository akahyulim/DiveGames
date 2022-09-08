#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>
#include <string>

namespace Editor
{
	struct OutModel
	{
		std::string outName;
		aiNode* pRootNode = nullptr;
		aiNode* pRootBone = nullptr;
		std::vector<unsigned int> meshIndices;	// urho�� hasset���� �����ߴ�.
		std::vector<aiMesh*> meshes;
		std::vector<aiNode*> meshNodes;
		std::vector<aiNode*> bones;
		std::vector<aiAnimation*> animations;
		uint32_t totalNumVertices = 0;
		uint32_t totalNumIndices = 0;
	};
	
	// ���� ������ �ܺο��� ������ �� �־�� �Ѵ�.
	// ���� �̸� ������� ������ �о� ���̰�,
	// �Ľ� �����͸� �̿��� �𵨷� �ͽ���Ʈ�ϴ� ���̴�.
	// ����Ƽ�� ��� ����â�� ���� �����ϰ�
	// �巡�� �� ������� �ε�� �ͽ���Ʈ�� ó���ϴ� �� �ϴ�.
	class ModelImporter
	{
	public:
		ModelImporter();
		~ModelImporter();

		bool LoadAndExportModel(const std::string& filepath);

		// ���� ����

	private:
		void collectMeshes(OutModel& model, aiNode* pNode);
		bool saveModel();

	private:
		aiScene* m_pScene;
		bool m_bAnimations;
		bool m_bMaterials;

		// ���� ������ �����ؾ� �Ѵ�.
	};
}