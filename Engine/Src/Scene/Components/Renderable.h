#pragma once
#include "Component.h"

namespace Dive
{
	class GameObejct;
	class Material;
	class Mesh;

	class Renderable : public Component
	{
	public:
		Renderable(GameObject* pGameObject);
		~Renderable() override;

		 bool HasMaterial() const { return m_pMaterial; }
		 Material* GetMaterial() const { return m_pMaterial; }
		 void SetMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }

		 Mesh* GetMesh() { return m_pMesh; }
		 void SetMesh(Mesh* pMesh) { m_pMesh = pMesh; }

	private:
	private:
		Material* m_pMaterial;
		Mesh* m_pMesh;
	};
}