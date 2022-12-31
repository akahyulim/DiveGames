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
		OutModel()
			: pRootNode(nullptr),
			pRootBone(nullptr),
			TotalVertexCount(0),
			TotalIndexCount(0)
		{}

		std::string OutName;
		aiNode* pRootNode;
		aiNode* pRootBone;
		std::set<uint32_t> MeshIndices;
		std::vector<aiMesh*> Meshes;
		std::vector<aiNode*> MeshNodes;
		std::vector<aiNode*> Bones;
		std::vector<aiNode*> PivotlessBones;
		std::vector<aiAnimation*> Animations;
		uint32_t TotalVertexCount;
		uint32_t TotalIndexCount;
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

		bool LoadAndExport(const std::string& filepath);

		// ���� ����
		
	private:
		void collectMeshes(OutModel& model, aiNode* pNode);
		void collectBones(OutModel& model);

		bool saveModel(OutModel& model);

		aiNode* getNode(const std::string& name, aiNode* pRootNode);

		// test �Լ�
		void printMeshData(OutModel& model);

	private:
		aiScene* m_pAiScene;
		bool m_bAnimations;
		bool m_bMaterials;

		// ���� ������ �����ؾ� �Ѵ�.
	};
}