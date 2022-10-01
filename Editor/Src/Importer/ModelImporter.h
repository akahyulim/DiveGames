#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>
#include <set>

namespace Editor
{
	struct OutModel
	{
		std::string outName;
		aiNode* pRootNode = nullptr;
		aiNode* pRootBone = nullptr;
		std::set<unsigned int> meshIndices;
		std::vector<aiMesh*> meshes;
		std::vector<aiNode*> meshNodes;
		std::vector<aiNode*> bones;
		std::vector<aiNode*> pivotlessBones;
		std::vector<aiAnimation*> animations;
		uint32_t totalNumVertices = 0;
		uint32_t totalNumIndices = 0;
	};
	
	// ���� ������ �ܺο��� ������ �� �־�� �Ѵ�.
	// ���� �̸� ������� ������ �о� ���̰�,
	// �Ľ� �����͸� �̿��� �𵨷� �ͽ���Ʈ�ϴ� ���̴�.
	// ����Ƽ�� ��� ����â�� ���� �����ϰ�
	// �巡�� �� ������� �ε�� �ͽ���Ʈ�� ó���ϴ� �� �ϴ�.
	//=> �����δ� Ư�� ������ ������ �ڵ����� ��ȯ�� ��?
	class ModelImporter
	{
	public:
		ModelImporter();
		~ModelImporter();

		bool LoadAndExportModel(const std::string& filepath);

		// ���� ����
		
	private:
		void collectMeshes(OutModel& model, aiNode* pNode);
		void collectBones(OutModel& model);

		bool saveModel(OutModel& model);

		aiNode* getNode(const std::string& name, aiNode* pRootNode);

		// test �Լ�
		void printMeshData(OutModel& model);

	private:
		aiScene* m_pScene;
		bool m_bAnimations;
		bool m_bMaterials;

		// ���� ������ �����ؾ� �Ѵ�.
	};
}