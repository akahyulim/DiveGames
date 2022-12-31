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
	
	// 각종 설정을 외부에서 변경할 수 있어야 한다.
	// 이후 이를 기반으로 파일을 읽어 들이고,
	// 파싱 데이터를 이용해 모델로 익스포트하는 것이다.
	// 유니티의 경우 설정창만 따로 존재하고
	// 드래그 앤 드랍으로 로드와 익스포트를 처리하는 듯 하다.
	//=> 실제로는 특정 폴더의 파일을 자동으로 변환할 듯?
	class ModelImporter
	{
	public:
		ModelImporter();
		~ModelImporter();

		bool LoadAndExport(const std::string& filepath);

		// 각종 설정
		
	private:
		void collectMeshes(OutModel& model, aiNode* pNode);
		void collectBones(OutModel& model);

		bool saveModel(OutModel& model);

		aiNode* getNode(const std::string& name, aiNode* pRootNode);

		// test 함수
		void printMeshData(OutModel& model);

	private:
		aiScene* m_pAiScene;
		bool m_bAnimations;
		bool m_bMaterials;

		// 설정 값들을 관리해야 한다.
	};
}