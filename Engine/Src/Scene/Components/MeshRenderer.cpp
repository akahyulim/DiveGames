#include "DivePch.h"
#include "MeshRenderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Graphics/Graphics.h"
#include "IO/Log.h"

namespace Dive
{
	MeshRenderer::MeshRenderer(GameObject* pGameObject)
		: Component(pGameObject)
	{
	}

	MeshRenderer::~MeshRenderer()
	{
	}
	
	void MeshRenderer::Draw()
	{
		// set shaders(Material이 가지는 편이 낫나...?)

		if (m_pMaterial)
		{
			// set texture
			// set constant buffer 
		}

		if (m_pMesh)
		{
			// set constant buffer
			// set buffers
		}

		// draw
	}
}