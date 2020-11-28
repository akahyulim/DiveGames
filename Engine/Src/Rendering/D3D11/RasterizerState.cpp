#include "DivePch.h"
#include "RasterizerState.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "RenderDevice.h"

namespace Dive
{

	RasterizerState::RasterizerState(const std::shared_ptr<RenderDevice>& device, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, BOOL depthClipEnable, BOOL scissorEnable,
		BOOL multiSampleEnable, BOOL antialiasedLineEnable)
	{
		if (!device->IsInitialized())
		{
			CORE_ERROR("");
			return;
		}

		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FrontCounterClockwise	= FALSE;
		desc.DepthBias				= 0;
		desc.DepthBiasClamp			= 0.0f;
		desc.SlopeScaledDepthBias	= 0.0f;
		desc.CullMode				= cullMode;
		desc.FillMode				= fillMode;
		desc.DepthClipEnable		= depthClipEnable;
		desc.ScissorEnable			= scissorEnable;
		desc.MultisampleEnable		= multiSampleEnable;
		desc.AntialiasedLineEnable	= antialiasedLineEnable;

		if (FAILED(device->GetD3dDevice()->CreateRasterizerState(&desc, &m_state)))
		{
			CORE_ERROR("");
			return;
		}

		m_bInitialized = true;
	}

	RasterizerState::~RasterizerState()
	{
		SAFE_RELEASE(m_state);
	}
}