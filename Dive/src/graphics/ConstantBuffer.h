#pragma once
#include "Core/CoreDefs.h"
#include "rendering/RenderDefs.h"

namespace Dive
{
	enum class eShaderStage
	{
		VertexShader,
		PixelShader,
	};

	class DvConstantBuffer
	{
	public:
		DvConstantBuffer(ID3D11Device* device, eVSConstantBufferSlot slot, size_t size);
		DvConstantBuffer(ID3D11Device* device, ePSConstantBufferSlot slot, size_t size);

		template<typename T>
		void Update(ID3D11DeviceContext* deviceContext, const T& data);
		void Bind(ID3D11DeviceContext* deviceContext);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
		UINT m_Slot;
		eShaderStage m_Stage;
	};


	template<typename T>
	void DvConstantBuffer::Update(ID3D11DeviceContext* deviceContext, const T& data)
	{
		assert(deviceContext);
		assert(m_Buffer);

		D3D11_MAPPED_SUBRESOURCE mappedData{};
		if (FAILED(deviceContext->Map(
			static_cast<ID3D11Resource*>(m_Buffer.Get()),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData)))
		{
			DV_LOG(ConstantBuffer, err, "ConstantBuffer의 Map에 실패");
			return;
		}

		std::memcpy(mappedData.pData, &data, sizeof(T));

		deviceContext->Unmap(m_Buffer.Get(), 0);
	}

	// ========================================================================================

	class ConstantBuffer
	{
	public:
		ConstantBuffer() = default;
		~ConstantBuffer();

		bool Create(UINT32 stride);
		void Release();

		void* Map();
		void Unmap();

		UINT32 GetStride() const { return m_Stride; }
		void SetStride(UINT32 stride) { m_Stride = stride; }

		ID3D11Buffer* GetBuffer() const { return m_Buffer; }

		static std::shared_ptr<ConstantBuffer> Generate(UINT32 stride);

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		UINT32 m_Stride = 0;
	};
}
