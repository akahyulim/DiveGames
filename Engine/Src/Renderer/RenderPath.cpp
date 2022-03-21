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
	
	// active scene이 있어야 한다.
	void RenderPath::Render()
	{
		// 이전 state 비교용
		CommandList cl;

		passDefault(&cl);
	}

	// 스파르탄의 경우 WorldResolve라는 이벤트를 통해 호출된다.
	void RenderPath::OnAcuireRenderObjects(std::vector<GameObject*>& gameObjects)
	{
		m_RenderObjects.clear();

		for (auto pGameObject : gameObjects)
		{
			// isActive도 추가?
			if (!pGameObject)
				continue;

			// 이러면 안된다. 구체 타입으로 저장되어 있을 거다.
			// 아니면, RenderComponent를 상속하는 모든 Component를 개별로 찾으면 되긴 할 듯...
			auto pRendererComponent = pGameObject->GetComponent<SpriteRenderer>();

			if (pRendererComponent)
			{
				// material로 opaque / transparent 구분
				bool bTransparent = false;

				m_RenderObjects[bTransparent ? eRenderObjectType::Transparent : eRenderObjectType::Opaque].emplace_back(pGameObject);
			}
		}

		// opaque, transparent의 경우 각각 vector 재정렬 함수 호출
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

		// 스파르탄은 이 함수의 인자로 전달받는다...
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