#pragma once
#include "Common.h"

namespace Dive
{
	enum class eShaderStage
	{
		VertexShader,
		PixelShader,
	};

	enum class eVSConstantBufferSlot : uint8_t
	{
		Object,
		Frame,
		Count
	};

	enum class ePSConstantBufferSlot : uint8_t
	{
		Camera,
		Material,
		Light,
		Count
	};

	class ConstantBuffer
	{
	public:
		ConstantBuffer(ID3D11Device* device, eVSConstantBufferSlot slot, uint32_t stride);
		ConstantBuffer(ID3D11Device* device, ePSConstantBufferSlot slot, uint32_t stride);

		template<typename T>
		bool Update(ID3D11DeviceContext* deviceContext, const T& data);
		void Bind(ID3D11DeviceContext* deviceContext);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		eVSConstantBufferSlot m_vsSlot = eVSConstantBufferSlot::Count;
		ePSConstantBufferSlot m_psSlot = ePSConstantBufferSlot::Count;
		eShaderStage m_shaderStage;
	};

	template<typename T>
	bool ConstantBuffer::Update(ID3D11DeviceContext* deviceContext, const T& data)
	{
		if (!m_buffer)
		{
			DV_LOG(ConstantBuffer, err, "[::Update] 버퍼 미생성");
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE mappedData{};
		auto hr = deviceContext->Map(
			static_cast<ID3D11Resource*>(m_buffer.Get()),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData);
		if (FAILED(hr))
		{
			DV_LOG(ConstantBuffer, err, "[::Update] Map 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		std::memcpy(mappedData.pData, &data, sizeof(T));
		deviceContext->Unmap(m_buffer.Get(), 0);

		return true;
	}
}
