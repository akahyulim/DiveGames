#pragma once
#include "Resource/Resource.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Graphics/GraphicsDefs.h"
#include "Scene/Components/Transform.h"
#include "Scene/GameObject.h"

namespace Dive
{
	// 노드 계층구조를 직접 관리
	struct NodeInfo
	{
		std::string name;
		DirectX::XMFLOAT4X4 transform;
		uint32_t numChildren;
		std::vector<NodeInfo> children;
	};

	// 기본적으로 Mesh를 관리하는 클래스다.
	// 따라서 Load, Save 등의 함수는 Mesh 정보를 파일에서 읽어 구성하고나 파일에 쓰는 작업으로 이루어져야 한다.
	// 문제는 각 Mesh가 노드를 가질 수 있다는 사실과
	// BoneInfoMap을 직접 가질 필요가 있는가 하는 것이다. => 이게 Animation과 연계되는 것도 좀 에바다.
	class Model : public Resource
	{
	public:
		Model();
		virtual ~Model();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		DvStaticMesh* InsertStaticMesh(DvStaticMesh* pMesh);
		SkinnedMesh* InsertSkinnedMesh(SkinnedMesh* pMesh);
	
		auto& GetBoneInfoMap() { return m_BoneInfoMap; }

		void BuildMeshBuffers();
		void Build();

		const auto& GetStaticMeshes() const { return m_StaticMeshes; }
		const auto& GetSkinnedMeshes() const { return m_SkinnedMeshes; }

		// test
		uint32_t GetNumStaticMeshes() const { return static_cast<uint32_t>(m_StaticMeshes.size()); }
		uint32_t GetNumSkinnedMeshes() const { return static_cast<uint32_t>(m_SkinnedMeshes.size()); }

		GameObject* GetRootGameObject() { return m_pRootGameObject; }
		void SetRootGameObject(GameObject* pRoot) { m_pRootGameObject = pRoot; }

		uint32_t GetBoneCount() { return static_cast<uint32_t>(m_BoneInfoMap.size()); }

		NodeInfo& GetRootNodeInfo() { return m_RootNodeInfo; }

	private:
		void buildHierarchy(NodeInfo* pNode, Transform* pParent);

	private:
		std::unordered_map<std::string, DvStaticMesh*> m_StaticMeshes;
		std::unordered_map<std::string, SkinnedMesh*> m_SkinnedMeshes;

		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		GameObject* m_pRootGameObject;
		
		NodeInfo m_RootNodeInfo;
	};
}