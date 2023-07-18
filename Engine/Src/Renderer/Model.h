#pragma once
#include "Resource/Resource.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Graphics/GraphicsDefs.h"
#include "Scene/Components/Transform.h"
#include "Scene/GameObject.h"

namespace Dive
{
	// ��� ���������� ���� ����
	struct NodeInfo
	{
		std::string name;
		DirectX::XMFLOAT4X4 transform;
		uint32_t numChildren;
		std::vector<NodeInfo> children;
	};

	// �⺻������ Mesh�� �����ϴ� Ŭ������.
	// ���� Load, Save ���� �Լ��� Mesh ������ ���Ͽ��� �о� �����ϰ� ���Ͽ� ���� �۾����� �̷������ �Ѵ�.
	// ������ �� Mesh�� ��带 ���� �� �ִٴ� ��ǰ�
	// BoneInfoMap�� ���� ���� �ʿ䰡 �ִ°� �ϴ� ���̴�. => �̰� Animation�� ����Ǵ� �͵� �� ���ٴ�.
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