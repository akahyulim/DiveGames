#pragma once
#include "RenderDefs.h"

namespace Dive
{
	class PipelineState
	{
	public:
		ID3D11SamplerState* GetOrCreateSamplerState(eSamplerType type);

	private:

	private:
		std::unordered_map<uint8_t, Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_SamplerCache;
	};
}
