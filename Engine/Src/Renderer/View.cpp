#include "divepch.h"
#include "View.h"
#include "Viewport.h"
#include "Renderer.h"
#include "RenderPath.h"
#include "Mesh.h"
#include "Model.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceCache.h"
#include "Scene/Scene.h"
#include "Scene/Component/StaticModel.h"
#include "Scene/GameObject.h"
#include "IO/Log.h"
#include "Math/Math.h"

namespace Dive 
{
	View::View(Context* pContext)
		: Object(pContext)
	{
		m_pGraphics = GetSubsystem<Graphics>();
		m_pRenderer = GetSubsystem<Renderer>();
	}

	View::~View()
	{
		DV_LOG_ENGINE_DEBUG("Destroy View");
	}

	void View::Update(float delta)
	{
		// 실제로는 octree와 cullCamera 이다.
		if (!m_pScene)
			return;

		// GetDrawables()
		// 일단 그려져야 할 오브젝트부터 추린다.
		// 옥트리와 컬링을 거친 오브젝트일 듯 하다.
		{
			m_Drawables.clear();

			auto allGameObjects = m_pScene->GetAllGameObjects();
			for (auto pGameObject : allGameObjects)
			{
				if (pGameObject->HasComponent<Dive::StaticModel>())
					m_Drawables.emplace_back(pGameObject->GetComponent<Dive::StaticModel>()->GetMesh(0));
			}
		}

		// GetBatches()
		// 오브젝트를 일단 라이트와 일반으로 분리한다.
		// 그리고 그려지는 방식에 따라 BatchGroup와 Batch로 나누어 관리한다.
		{
			// Scene이 Component를 가지고 관리 중이므로
			// Light와 Model Component를 가져오면 될 듯 하다.
			// 문제는 vector 단위가 아니라 index로 개별 component 접근만 가능하다.
			// 물론 이는 vector를 참조리턴하는 함수를 추가하면 된다.
			// urho는 octree component로 어찌하는 듯 하다.

			// spartan은 renderer에서 object type별 entity vector를 관리한다.
			// 그리고 매 프레임 scene으로부터 entity를 가져와 이를 갱신하며,
			// pass에서 entity를 통해 component를 확인하여 그린다.
		}
	}
	
	void View::Render()
	{
		// clear
		{
			m_pCurrentRenderTarget = m_pRenderTarget;

			//eClearTarget flags = eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil;
			m_pGraphics->SetRenderTarget(0, dynamic_cast<Texture2D*>(m_pCurrentRenderTarget));
			m_pGraphics->Clear(0, DirectX::XMFLOAT4(1.0f, 1.0f, 0.6f, 1.0f), 1.0f, 0);

			// 이건 임시 위치
			m_pGraphics->SetViewportRect(m_ViewRect);
		}

		// Draw
		{
			auto pDeviceContext = m_pGraphics->GetDeviceContext();

			for(const auto* pMesh : m_Drawables)
			{
				// set vertex buffer
				m_pGraphics->SetVertexBuffer(pMesh->GetVertexBuffer(0));
				
				// set index buffer
				m_pGraphics->SetIndexBuffer(pMesh->GetIndexBuffer());

				// set shaders & inputlayout
				m_pGraphics->SetDefaultShader();

				// draw
				// Mesh의 DrawRange를 구현하지 않았다.
				auto indexCount = 3;// pMesh->GetIndexCount();
				auto indexStart = 0;// pMesh->GetIndexStart();
				auto primitiveType = pMesh->GetPrimitiveType();
				m_pGraphics->DrawIndexed(primitiveType, indexCount, indexStart);
			}
		}
	}
	
	bool View::Define(Texture* pRenderTarget, Viewport* pViewport)
	{
		m_pRenderTarget = pRenderTarget;

		// pViewport로부터 RenderPath를 획득
		m_pScene = pViewport->GetScene();
		// 카메라도 획득

		int width = pRenderTarget ? pRenderTarget->GetWidth() : m_pGraphics->GetWidth();
		int height = pRenderTarget ? pRenderTarget->GetHeight() : m_pGraphics->GetHeight();
		m_RenderTargetSize = DirectX::XMINT2(width, height);

		auto rect = pViewport->GetRect();
		if (rect.left == 0 && rect.right == 0 && rect.top == 0 && rect.bottom == 0)
		{
			m_ViewRect.left = 0;
			m_ViewRect.right = width;
			m_ViewRect.top = 0;
			m_ViewRect.bottom = height;
		}
		else
		{
			m_ViewRect.left = Math::Clamp<int>(rect.left, 0, width - 1);
			m_ViewRect.right = Math::Clamp<int>(rect.right, m_ViewRect.left + 1, width);
			m_ViewRect.top = Math::Clamp<int>(rect.top, 0, height - 1);
			m_ViewRect.bottom = Math::Clamp<int>(rect.bottom, m_ViewRect.top + 1, height);
		}
		m_ViewSize = DirectX::XMINT2(m_ViewRect.right - m_ViewRect.left, m_ViewRect.bottom - m_ViewRect.top);

		return true;
	}
}