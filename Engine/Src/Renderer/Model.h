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

		GameObject* GetRootBone() const { return m_pRootBone; }
		void SetRootBone(GameObject* pRootBone) { m_pRootBone = pRootBone; }

	private:
		// 스파르탄은 메시(구 모델)에서 단일 버퍼를 관리하고
		// 렌더러블에서 각자 자신이 그릴 파트의 오프셋을 저장한다.
		// 반면 urho3d는 모델에서 통합 또는 개별 버퍼를 관리하고
		// 지오메트리에서 자신이 그릴 파트의 오브셋을 저장하며
		// 스태틱, 애니메이션 모델 컴포넌트에서 지오메트리들을 가진다.
		// 즉, 하나의 모델 컴포넌트가 모든 지오메트리를 관리하며 이때문에 계층구조는 따로 필요치 않게 된다.

		// 0. 현재 메시를 urho3d의 지오메트리로 변경할 것인가?
		// 1. 버퍼를 특정 상황에 맞춰 나눌 것인가?
		// 2. 정점 버퍼를 하나의 클래스로 만들 수 있나?
		// => 일단 유니티를 참조하자면 지오메트리보단 메시라는 이름이 어울리고
		// 메시에는 버퍼와 오프셋을, 렌더러 컴포넌트는 이를 이용해 그리는 Draw로 나누는 편이 나아보인다.
		std::unordered_map<std::string, StaticMesh*> m_StaticMeshes;
		std::unordered_map<std::string, SkinnedMesh*> m_SkinnedMeshes;

		Skeleton m_Skeleton;
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		GameObject* m_pRootGameObject;
		GameObject* m_pRootBone;
	};
}