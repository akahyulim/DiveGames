#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Scene/Component/Drawable.h"

namespace Dive
{
	class View;
	class Pass;
	class ShaderVariation;

	struct BatchRenderer
	{
		BatchRenderer() = default;
		BatchRenderer(const DrawableBatch& batch)
			: m_pMesh(batch.m_pMesh),
			m_pMaterial(batch.m_pMaterial)
		{
		}

		// view�� camera�� �޴´�.
		void Prepare(View* pView);
		// ���� view�� camera�� �޴´�.
		void Draw(View* pView);

		Mesh* m_pMesh = nullptr;
		Material* m_pMaterial = nullptr;
		Pass* m_pPass = nullptr;
		ShaderVariation* m_pVertexShader = nullptr;
		ShaderVariation* m_pPixelShader = nullptr;
	};
}