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

		// ������ bind�� ��ġ�� �ȵȴ�.
		// �׷��ٰ� bind �Լ��� ����� ���� ���� �Լ��� �ȴ�.
		ID3D11SamplerState* GetState() { return m_state.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_state;
	};
}