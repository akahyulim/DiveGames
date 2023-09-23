#pragma once
#include "Transform.h"
#include "MeshRenderer.h"

namespace Dive
{
	class IMesh;
	class SkinnedMesh;
	struct ModelVertexShaderBuffer;	// �ӽ�
	struct BoneInfo;	// ���� �ӽ�
	class Animation;

	class SkinnedMeshRenderer : public IMeshRenderer
	{
	public:
		SkinnedMeshRenderer(GameObject* pGameObject);
		virtual ~SkinnedMeshRenderer();

		void Update() override;
		void Draw() override;

		IMesh* GetMesh() override;
		void SetMesh(IMesh* pMesh) override;

		// ����Ƽ���� ��������� �迭�� ����������
		// ui���� root bone�� �ٷ�� �ִ�.
		//Transform* GetRootBone();
		//Transform* GetBone(uint32_t index);
		//void SetBone(uint32_t index, Transform transform);
		//void InsertBone(Transform* pTransform);
		void InsertBone(BoneInfo* pBone);
		void SetBones(const std::vector<BoneInfo>& bones);


	private:
		void calcuBoneTransform(Transform* pNode, DirectX::XMFLOAT4X4 parent);

	private:
		SkinnedMesh* m_pMesh;
		//std::vector<Transform*> m_Bones;
		std::vector<BoneInfo> m_Bones;
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		std::vector<DirectX::XMFLOAT4X4> m_SkinMatrices;
		float m_CurrentTime;

		Animation* m_pCurrentAnimation;
	};
}