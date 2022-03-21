#include "divepch.h"
#include "RenderPath.h"
#include "PipelineState.h"
#include "CommandList.h"
#include "Renderer.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Renderer.h"
#include "Scene/Component/SpriteRenderer.h"

namespace Dive
{
	void RenderPath::Update(float delta)
	{
	}
	
	// active scene�� �־�� �Ѵ�.
	void RenderPath::Render()
	{
		// ���� state �񱳿�
		CommandList cl;

		passDefault(&cl);
	}

	// ���ĸ�ź�� ��� WorldResolve��� �̺�Ʈ�� ���� ȣ��ȴ�.
	void RenderPath::OnAcuireRenderObjects(std::vector<GameObject*>& gameObjects)
	{
		m_RenderObjects.clear();

		for (auto pGameObject : gameObjects)
		{
			// isActive�� �߰�?
			if (!pGameObject)
				continue;

			// �̷��� �ȵȴ�. ��ü Ÿ������ ����Ǿ� ���� �Ŵ�.
			// �ƴϸ�, RenderComponent�� ����ϴ� ��� Component�� ������ ã���� �Ǳ� �� ��...
			auto pRendererComponent = pGameObject->GetComponent<SpriteRenderer>();

			if (pRendererComponent)
			{
				// material�� opaque / transparent ����
				bool bTransparent = false;

				m_RenderObjects[bTransparent ? eRenderObjectType::Transparent : eRenderObjectType::Opaque].emplace_back(pGameObject);
			}
		}

		// opaque, transparent�� ��� ���� vector ������ �Լ� ȣ��
	}

	void RenderPath::passDefault(CommandList* pCl)
	{
		PipelineState ps;
		ps.primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ps.pInputLayout			= Renderer::GetShader(eShaderType::Sprite)->pInputLayout;
		ps.pVertexShader		= Renderer::GetShader(eShaderType::Sprite)->pVertexShader;
		ps.pRasterizerState		= Renderer::GetRasterizerState(eRasterizerStateType::CullBackSolid);
		ps.pPixelShader			= Renderer::GetShader(eShaderType::Sprite)->pPixelShader;
		ps.pDepthStencilState	= Renderer::GetDepthStencilState(eDepthStencilStateType::DepthOnStencilOn);
		pCl->SetPipelineState(ps);

		// ���ĸ�ź�� �� �Լ��� ���ڷ� ���޹޴´�...
		for(auto pOpaqueObject : m_RenderObjects[eRenderObjectType::Opaque])
		{
			// vertex buffer
			// index buffer
			// constant buffer
			// shader resource view

			// draw
		}
	}
}