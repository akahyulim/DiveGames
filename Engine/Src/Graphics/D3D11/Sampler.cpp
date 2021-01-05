#include "DivePch.h"
#include "Sampler.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Core/Context.h"
#include "../Graphics.h"


namespace Dive
{
	Sampler::Sampler(Context * context, const D3D11_FILTER textureFilter, const D3D11_TEXTURE_ADDRESS_MODE addressMode, const D3D11_COMPARISON_FUNC cmpFunc, UINT maxAnisotropy)
		: Object(context)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
		{
			CORE_ERROR("");
			return;
		}

		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter			= textureFilter;
		desc.AddressU		= addressMode;
		desc.AddressV		= addressMode;
		desc.AddressW		= addressMode;
		desc.ComparisonFunc = cmpFunc;
		desc.MaxAnisotropy	= maxAnisotropy;
		desc.MaxLOD			= FLT_MAX;
		desc.MinLOD			= FLT_MIN;
		desc.MipLODBias		= 0.0f;
		desc.BorderColor[0] = 0.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 0.0f;
		desc.BorderColor[3] = 0.0f;

		if (FAILED(graphics->GetRHIDevice()->CreateSamplerState(&desc, &m_state)))
		{
			CORE_ERROR("");
		}
	}

	Sampler::~Sampler()
	{
		SAFE_RELEASE(m_state);
	}
}
