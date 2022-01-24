#include "dvMeshRenderer.h"
#include "Transform.h"
#include "../GameObject.h"
#include "Renderer/dvMesh.h"
#include "Renderer/Renderer.h"

namespace DiveEngine
{
	dvMeshRenderer::dvMeshRenderer(GameObject* pGameObject)
		: Component(pGameObject),
		m_pMesh(nullptr)
	{
		//m_Type = eComponentType::dvMeshRenderer;
	}

	dvMeshRenderer::~dvMeshRenderer()
	{
	}
}