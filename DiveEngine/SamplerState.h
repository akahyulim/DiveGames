#pragma once
#include "GraphicsDevice.h"
#include "Renderer.h"

namespace Dive
{
	class SamplerState
	{
	public:
		SamplerState(ID3D11Device* device, D3D11_SAMPLER_DESC* desc)
		{
			assert(device != nullptr);
			assert(desc != nullptr);

			auto hResult = device->CreateSamplerState(desc, &m_state);

			assert(SUCCEEDED(hResult));
		}
		~SamplerState() = default;

		// 생성과 bind의 일치가 안된다.
		// 그렇다고 bind 함수를 만들면 역시 랩핑 함수가 된다.
		ID3D11SamplerState* GetState() { return m_state.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_state;
	};
}