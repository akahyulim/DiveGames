#include "divepch.h"
#include "View.h"
#include "Viewport.h"
#include "Renderer.h"
#include "RenderPath.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceCache.h"
#include "Scene/Scene.h"
#include "Scene/Component/StaticModel.h"
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
		DV_LOG_ENGINE_DEBUG("View::Update");
	}
	
	void View::Render()
	{
		DV_LOG_ENGINE_DEBUG("View::Render: {0:d} x {1:d}", m_RenderTargetSize.x, m_RenderTargetSize.y);

		// clear
		{
			m_pCurrentRenderTarget = m_pRenderTarget;

			//eClearTarget flags = eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil;
			m_pGraphics->SetRenderTarget(0, dynamic_cast<Texture2D*>(m_pCurrentRenderTarget));
			m_pGraphics->Clear(0, DirectX::XMFLOAT4(1.0f, 1.0f, 0.6f, 1.0f), 1.0f, 0);
		}
	}
	
	bool View::Define(Texture* pRenderTarget, Viewport* pViewport)
	{
		m_pRenderTarget = pRenderTarget;

		// pViewport·ÎºÎÅÍ RenderPath¸¦ È¹µæ
		m_pScene = pViewport->GetScene();
		// Ä«¸Þ¶óµµ È¹µæ

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