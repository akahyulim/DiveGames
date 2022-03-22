#include "divepch.h"
#include "RenderPath.h"
#include "PipelineState.h"
#include "CommandList.h"
#include "Renderer.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/SpriteRenderable.h"
#include "Events/EngineEvents.h"

namespace Dive
{
	RenderPath::RenderPath()
	{
		SUBSCRIBE_EVENT(eEventType::SceneResolve, EVENT_HANDLER(OnAcquireRenderableObjects));
	}

	RenderPath::~RenderPath()
	{
		UNSUBSCRIBE_EVENT(eEventType::SceneResolve, EVENT_HANDLER(OnAcquireRenderableObjects));
	}

	void RenderPath::Update(float delta)
	{
	}
	
	void RenderPath::Render()
	{
		// ���� state �񱳿�
		CommandList cl;

		passDefault(&cl);
	}

	void RenderPath::OnAcquireRenderableObjects(const Event& e)
	{
		m_RenderableObjects.clear();

		const SceneResolveEvent& sceneResolveEvent = dynamic_cast<const SceneResolveEvent&>(e);
		auto pGameObjects = sceneResolveEvent.GetGameObjects();

		for (auto pGameObject : *pGameObjects)
		{
			if (!pGameObject || !pGameObject->IsActive())
				continue;

			// �̷��� �ϸ� Pass���� Component�� ���� Buffer ���� ���� �� ����...
			// ���� Base�� Renderable�� �ް� Interface�� ������ �ϴ� ���� ����ϴ�...
			auto pSpriteRenderer = pGameObject->GetComponent<SpriteRenderable>();

			if (pSpriteRenderer)
			{
				// material�� opaque / transparent ����
				bool bTransparent = false;

				m_RenderableObjects[bTransparent ? eRenderableType::Transparent : eRenderableType::Opaque].emplace_back(pGameObject);
			}
		}

		// opaque, transparent�� ��� ���� vector ���� �Լ� ȣ��
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

		// ���ĸ�ź�� RenderObject Type�� ���޹޴´�.
		for(auto pOpaqueObject : m_RenderableObjects[eRenderableType::Opaque])
		{
			// vertex buffer
			// index buffer
			// constant buffer
			// shader resource view

			// draw
		}
	}
}