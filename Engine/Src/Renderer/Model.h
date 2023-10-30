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

		// ������ �� ���̴�. => boneInfoMap���� ��ä����.
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
		// ���ĸ�ź�� �޽�(�� ��)���� ���� ���۸� �����ϰ�
		// ���������� ���� �ڽ��� �׸� ��Ʈ�� �������� �����Ѵ�.
		// �ݸ� urho3d�� �𵨿��� ���� �Ǵ� ���� ���۸� �����ϰ�
		// ������Ʈ������ �ڽ��� �׸� ��Ʈ�� ������� �����ϸ�
		// ����ƽ, �ִϸ��̼� �� ������Ʈ���� ������Ʈ������ ������.
		// ��, �ϳ��� �� ������Ʈ�� ��� ������Ʈ���� �����ϸ� �̶����� ���������� ���� �ʿ�ġ �ʰ� �ȴ�.

		// 0. ���� �޽ø� urho3d�� ������Ʈ���� ������ ���ΰ�?
		// 1. ���۸� Ư�� ��Ȳ�� ���� ���� ���ΰ�?
		// 2. ���� ���۸� �ϳ��� Ŭ������ ���� �� �ֳ�?
		// => �ϴ� ����Ƽ�� �������ڸ� ������Ʈ������ �޽ö�� �̸��� ��︮��
		// �޽ÿ��� ���ۿ� ��������, ������ ������Ʈ�� �̸� �̿��� �׸��� Draw�� ������ ���� ���ƺ��δ�.
		std::unordered_map<std::string, StaticMesh*> m_StaticMeshes;
		std::unordered_map<std::string, SkinnedMesh*> m_SkinnedMeshes;

		Skeleton m_Skeleton;
		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		GameObject* m_pRootGameObject;
		GameObject* m_pRootBone;
	};
}