#pragma once
#include "Common.h"
#include "Graphics.h"

namespace Dive
{
	enum class eCBufferSlotVS : uint8_t
	{
		Camera = 0,
		Object = 1,
		Count
	};

	enum class eCBufferSlotPS : uint8_t
	{
		Camera = 0,
		Material = 1,
		Light = 2,
		Count
	};

	class ConstantBuffer
	{
	public:
		ConstantBuffer(uint32_t stride);

		template<typename T>
		bool Update(const T& data);
		
		void BindVS(eCBufferSlotVS slot);
		void BindPS(eCBufferSlotPS slot);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	};

	template<typename T>
	bool ConstantBuffer::Update(const T& data)
	{
		if (!m_buffer)
		{
			DV_LOG(ConstantBuffer, err, "[::Update] 버퍼 미생성");
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE mappedData{};
		auto hr = Graphics::GetDeviceContext()->Map(
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
		Graphics::GetDeviceContext()->Unmap(m_buffer.Get(), 0);

		return true;
	}
}
