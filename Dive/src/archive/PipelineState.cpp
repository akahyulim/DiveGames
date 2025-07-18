#include "stdafx.h"
#include "PipelineState.h"
#include "graphics/Graphics.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
    static D3D11_SAMPLER_DESC GetSamplerDesc(eSamplerType type) 
    {
        D3D11_SAMPLER_DESC desc = {};
        desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        switch (type) 
        {
        case eSamplerType::LinearWrap:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case eSamplerType::LinearClamp:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            break;
        case eSamplerType::PointWrap:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case eSamplerType::PointClamp:
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            break;
        case eSamplerType::AnisotropicWrap:
            desc.Filter = D3D11_FILTER_ANISOTROPIC;
            desc.MaxAnisotropy = 16;
            break;
        case eSamplerType::ShadowComparison:
            desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
            desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
            desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;
            break;
        default:
            assert(false && "Unknown sampler type");
        }

        return desc;
    }

	ID3D11SamplerState* PipelineState::GetOrCreateSamplerState(eSamplerType type)
	{
		auto it = m_SamplerCache.find(static_cast<uint8_t>(type));
		if (it != m_SamplerCache.end())
			return it->second.Get();

		auto desc = GetSamplerDesc(type);
		ComPtr<ID3D11SamplerState> sampler;
		Graphics::GetDevice()->CreateSamplerState(&desc, &sampler);
        m_SamplerCache[static_cast<uint8_t>(type)] = sampler;
		
		return sampler.Get();
	}
}
