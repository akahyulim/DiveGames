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
		// 이전 state 비교용
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

			// 이렇게 하면 Pass에서 Component를 통해 Buffer 등을 얻을 수 없다...
			// 역시 Base인 Renderable로 받고 Interface를 사용토록 하는 것이 깔끔하다...
			auto pSpriteRenderer = pGameObject->GetComponent<SpriteRenderable>();

			if (pSpriteRenderer)
			{
				// material로 opaque / transparent 구분
				bool bTransparent = false;

				m_RenderableObjects[bTransparent ? eRenderableType::Transparent : eRenderableType::Opaque].emplace_back(pGameObject);
			}
		}

		// opaque, transparent의 경우 각각 vector 정렬 함수 호출
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

		// 스파르탄은 RenderObject Type을 전달받는다.
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