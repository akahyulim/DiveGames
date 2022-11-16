#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;

	class IndexBuffer : public Object
	{
		DIVE_OBJECT(IndexBuffer, Object)

	public:
		explicit IndexBuffer(Context* pContext);
		~IndexBuffer() override;

		template<typename T> bool Create(const std::vector<T>& indices);
		template<typename T> bool CreateDynamic(unsigned int count);

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_pBuffer.Get(); }
		
		unsigned int GetCount() const { return m_Count; }
		unsigned int GetStride() const { return m_Stride; }
		
		DXGI_FORMAT GetFormat() const { return m_Stride == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }
		
		bool IsDynamic() const { return m_bDynamic; }

	private:
		bool create(const void* pData);

	private:
		Graphics* m_pGraphics = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
		unsigned int m_Count = 0;
		unsigned int m_Stride = 0;
		bool m_bDynamic = false;
	};

	template<typename T> 
	bool IndexBuffer::Create(const std::vector<T>& indices)
	{
		m_bDynamic = false;
		m_Stride = static_cast<unsigned int>(sizeof(T));
		m_Count = static_cast<unsigned int>(indices.size());

		return create(static_cast<const void*>(indices.data()));
	}

	template<typename T> 
	bool IndexBuffer::CreateDynamic(unsigned int count)
	{
		m_bDynamic = true;
		m_Stride = static_cast<unsigned int>(sizeof(T));
		m_Count = count;

		return create(nullptr);
	}
}