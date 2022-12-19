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
		template<typename T> bool CreateDynamic(uint32_t count);

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		
		uint32_t GetCount() const { return m_Count; }
		uint32_t GetStride() const { return m_Stride; }
		
		DXGI_FORMAT GetFormat() const { return m_Stride == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }
		
		bool IsDynamic() const { return m_bDynamic; }

	private:
		bool create(const void* pData);

	private:
		Graphics* m_pGraphics;
		ID3D11Buffer* m_pBuffer;
		uint32_t m_Count;
		uint32_t m_Stride;
		bool m_bDynamic;
	};

	template<typename T> 
	bool IndexBuffer::Create(const std::vector<T>& indices)
	{
		m_bDynamic = false;
		m_Stride = static_cast<uint32_t>(sizeof(T));
		m_Count = static_cast<uint32_t>(indices.size());

		return create(static_cast<const void*>(indices.data()));
	}

	template<typename T> 
	bool IndexBuffer::CreateDynamic(uint32_t count)
	{
		m_bDynamic = true;
		m_Stride = static_cast<uint32_t>(sizeof(T));
		m_Count = count;

		return create(nullptr);
	}
}