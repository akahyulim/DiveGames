#include "DivePch.h"
#include "BlendState.h"
#include "RenderDevice.h"
#include "Core/Context.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


namespace Dive
{
	BlendState::BlendState(Context* context, const std::shared_ptr<RenderDevice>& device, BOOL enable, D3D11_BLEND source, D3D11_BLEND dest, D3D11_BLEND_OP op,
		D3D11_BLEND sourceAlpha, D3D11_BLEND destAlpha, D3D11_BLEND_OP opAlpha, float* blendFactor)
		: Object(context), m_state(nullptr)
	{
		if (!device->IsInitialized())
		{
			CORE_ERROR("");
			return;
		}

		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AlphaToCoverageEnable	= false;
		desc.IndependentBlendEnable = enable;
		for (auto& renderTarget : desc.RenderTarget)
		{
			renderTarget.BlendEnable			= enable;
			renderTarget.SrcBlend				= source;
			renderTarget.DestBlend				= dest;
			renderTarget.BlendOp				= op;
			renderTarget.SrcBlendAlpha			= sourceAlpha;
			renderTarget.DestBlendAlpha			= destAlpha;
			renderTarget.BlendOpAlpha			= opAlpha;
			renderTarget.RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		if (FAILED(device->GetD3dDevice()->CreateBlendState(&desc, &m_state)))
		{
			CORE_ERROR("");
			return;
		}

		if (blendFactor)
		{
			m_blendFactor[0] = blendFactor[0];
			m_blendFactor[1] = blendFactor[1];
			m_blendFactor[2] = blendFactor[2];
			m_blendFactor[3] = blendFactor[3];
		}

		m_bInitialized = true;
	}

	BlendState::~BlendState()
	{
		SAFE_RELEASE(m_state);
	}
}