#pragma once
#include "Component.h"

namespace Dive
{
	class Mesh;
	class Material;

	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(GameObject* pGameObject);
		virtual ~MeshRenderer();

		void Draw();

	private:
	private:
		Mesh* m_pMesh;
		Material* m_pMaterial;
	};
}