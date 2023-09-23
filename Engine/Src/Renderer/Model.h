#pragma once
#include "Resource/Resource.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Skeleton.h"
#include "Graphics/GraphicsDefs.h"
#include "Scene/Components/Transform.h"
#include "Scene/GameObject.h"

namespace Dive
{
	class Model : public Resource
	{
	public:
		Model();
		virtual ~Model();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		StaticMesh* InsertStaticMesh(StaticMesh* pMesh);
		SkinnedMesh* InsertSkinnedMesh(SkinnedMesh* pMesh);
	
		void BuildMeshBuffers();
		void Build();

		const auto& GetStaticMeshes() const { return m_StaticMeshes; }
		const auto& GetSkinnedMeshes() const { return m_SkinnedMeshes; }

		// 관리만 할 뿐이다. => boneInfoMap으로 대채하자.
		Skeleton& GetSkeleton() { return m_Skeleton; }
		void SetSkeleton(const Skeleton& skeleton) { m_Skeleton = skeleton; }

		std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
		uint32_t GetBoneCount();

		// test
		uint32_t GetNumStaticMeshes() const { return static_cast<uint32_t>(m_StaticMeshes.size()); }
		uint32_t GetNumSkinnedMeshes() const { return static_cast<uint32_t>(m_SkinnedMeshes.size()); }

		GameObject* GetRootGameObject() { return m_pRootGameObject; }
		void SetRootGameObject(GameObject* pRoot) { m_pRootGameObject = pRoot; }

	private:
		std::unordered_map<std::string, StaticMesh*> m_StaticMeshes;
		std::unordered_map<std::string, SkinnedMesh*> m_SkinnedMeshes;

		Skeleton m_Skeleton;
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		GameObject* m_pRootGameObject;
	};
}