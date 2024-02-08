#pragma once
#include "DvComponent.h"

namespace Dive
{
	class DvGameObejct;
	class DvMaterial;
	class DvMesh;

	class Renderable : public DvComponent
	{
	public:
		Renderable(DvGameObject* pGameObject);
		~Renderable() override;

		 bool HasMaterial() const { return m_pMaterial; }
		 DvMaterial* GetMaterial() const { return m_pMaterial; }
		 void SetMaterial(DvMaterial* pMaterial) { m_pMaterial = pMaterial; }

		 DvMesh* GetMesh() { return m_pMesh; }
		 void SetMesh(DvMesh* pMesh) { m_pMesh = pMesh; }

	private:
	private:
		DvMaterial* m_pMaterial;
		DvMesh* m_pMesh;
	};
}