#include "DivePch.h"
#include "View.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "IO/Log.h"
#include "Scene/Scene.h"

namespace Dive
{
	View::View()
	{
	}

	View::~View()
	{
		DV_CORE_TRACE("View º“∏Í¿⁄ »£√‚");
	}

	void View::Update(float delta)
	{
	}

	void View::Render()
	{
		// Clear
		{
			auto* pRTV = Graphics::GetDefaultRenderTargetView();
			auto* pDSV = Graphics::GetDefaultDepthStencilView();

			Graphics::SetRenderTargetViews(0, pRTV);
			Graphics::SetDepthStencilView(pDSV);

			Graphics::ClearRenderTargets(eClearTarget::Color | eClearTarget::Depth, DirectX::XMFLOAT4(0.75f, 0.75f, 1.0f, 1.0f), 1.0f, 0);
		}
	}
}