#pragma once
#include "Transform.h"
#include "MeshRenderer.h"

namespace Dive
{
	class Mesh;
	class SkinnedMesh;
	struct ModelVertexShaderBuffer;	// 임시
	struct BoneInfo;	// 역시 임시
	class Animation;

	class SkinnedMeshRenderer : public IMeshRenderer
	{
	public:
		SkinnedMeshRenderer(GameObject* pGameObject);
		virtual ~SkinnedMeshRenderer();

		void Update() override;
		void Draw() override;

		Mesh* GetMesh() override;
		void SetMesh(Mesh* pMesh) override;

		// 유니티에서 멤버변수는 배열로 관리하지만
		// ui에선 root bone을 다루고 있다.
		//Transform* GetRootBone();
		//Transform* GetBone(uint32_t index);
		//void SetBone(uint32_t index, Transform transform);
		//void InsertBone(Transform* pTransform);
		void InsertBone(BoneInfo* pBone);
		void SetBones(const std::vector<BoneInfo>& bones);

		GameObject* GetRootBone() const { return m_pRootBone; }
		void SetRootBone(GameObject* pRootBone) { m_pRootBone = pRootBone; }

	private:
		void calcuBoneTransform(Transform* pNode, DirectX::XMFLOAT4X4 parent);

	private:
		SkinnedMesh* m_pMesh;
		//std::vector<Transform*> m_Bones;
		std::vector<BoneInfo> m_Bones;
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		std::vector<DirectX::XMFLOAT4X4> m_SkinMatrices;
		float m_CurrentTime;

		GameObject* m_pRootBone;

		Animation* m_pCurrentAnimation;
	};
}