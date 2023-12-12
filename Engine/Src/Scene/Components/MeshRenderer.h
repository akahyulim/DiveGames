#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	class IMesh;
	class StaticMesh;
	class Material;

	// �̸��� �� �ƽ���.
	// �׸��� ����Ƽ�� ��� SkinnedMeshRenderer�� MeshRenderer�� ����Ѵ�.
	// => �̴� StaticMesh�� SkinnnedMesh�� ���еǾ� ���� �ʱ� ������ �� �ϴ�.
	// => ��, VertexBuffer�� ���о��� ������ �� �ִٸ� �� ���� Mesh�� �� ���� Ÿ������ ������ �� �ִ�.
	// => �̸� ���ؼ� ���۸� �̸� �����س��� ��/����� ���� �����͸� �����ϸ� �ȴ�.
	// => �ٸ� �� ��� ��ǲ���̾ƿ��� ������ ������ �����ؾ� �Ѵ�.
	class IMeshRenderer : public Component
	{
	public:
		IMeshRenderer(GameObject* pGameObject);
		virtual ~IMeshRenderer();

		virtual void Draw() = 0;

		virtual IMesh* GetMesh() = 0;
		virtual void SetMesh(IMesh* pMesh) = 0;

		bool HasMaterial() const { return m_pMaterial; }
		Material* GetMaterial() const { return m_pMaterial; }
		void SetMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }

	protected:
		Material* m_pMaterial;
	};

	class MeshRenderer : public IMeshRenderer
	{
	public:
		MeshRenderer(GameObject* pGameObject);
		virtual ~MeshRenderer();

		void Draw() override;

		IMesh* GetMesh() override;
		void SetMesh(IMesh* pMesh) override;

	private:

	private:
		StaticMesh* m_pMesh;
	};
}