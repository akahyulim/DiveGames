#include "DivePch.h"
#include "BlendState.h"
#include "RenderDevice.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


namespace Dive
{
	BlendState::BlendState(RenderDevice* device, BOOL enable, D3D11_BLEND source, D3D11_BLEND dest, D3D11_BLEND_OP op,
		D3D11_BLEND sourceAlpha, D3D11_BLEND destAlpha, D3D11_BLEND_OP opAlpha, float* blendFactor)
		: m_State(nullptr)
	{
		if (!device || !device->GetD3dDevice())
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

		if (FAILED(device->GetD3dDevice()->CreateBlendState(&desc, &m_State)))
		{
			CORE_ERROR("");
			return;
		}

		if (blendFactor)
		{
			m_BlendFactor[0] = blendFactor[0];
			m_BlendFactor[1] = blendFactor[1];
			m_BlendFactor[2] = blendFactor[2];
			m_BlendFactor[3] = blendFactor[3];
		}

		m_bInitialized = true;
	}

	BlendState::~BlendState()
	{
		SAFE_RELEASE(m_State);
	}
}