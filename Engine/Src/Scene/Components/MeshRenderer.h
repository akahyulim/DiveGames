#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	class IMesh;
	class DvStaticMesh;
	class Material;

	// 이름이 좀 아쉽다.
	// 그리고 유니티의 경우 SkinnedMeshRenderer는 MeshRenderer를 상속한다.
	class IMeshRenderer : public Component
	{
	public:
		IMeshRenderer(GameObject* pGameObject);
		virtual ~IMeshRenderer();

		virtual void Draw() const = 0;

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

		void Draw() const override;

		IMesh* GetMesh() override;
		void SetMesh(IMesh* pMesh) override;

	private:

	private:
		DvStaticMesh* m_pMesh;
	};
}