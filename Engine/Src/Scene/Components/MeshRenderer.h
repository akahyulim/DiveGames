#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	class Mesh;
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

		virtual Mesh* GetMesh() = 0;
		virtual void SetMesh(Mesh* pMesh) = 0;

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

		Mesh* GetMesh() override;
		void SetMesh(Mesh* pMesh) override;

	private:

	private:
		StaticMesh* m_pMesh;
	};
}