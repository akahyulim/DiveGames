#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Context;
	class Graphics;

	class VertexBuffer : public Object
	{
		DIVE_OBJECT(VertexBuffer, Object)

	public:
		VertexBuffer(Context* pContext);
		~VertexBuffer() override;

		bool SetSize(unsigned int vertexCount, const std::vector<VertexElement>& elements, bool bDynamic = false);
		bool SetData(void* pData);

		template<typename T> bool Create(const std::vector<T>& vertices);
		template<typename T> bool CreateDynamic(unsigned int count);

		void* Map();
		void Unmap();

		unsigned char* GetData() const { return m_pData; }

		ID3D11Buffer* GetBuffer() const { return m_pBuffer.Get(); }

		unsigned int GetVertexCount() const { return m_Count; }
		unsigned int GetVertexSize() const { return m_Stride; }
		
		const std::vector<VertexElement>& GetElements() const { return m_Elements; }
		
		// VertexBuffers의 인덱스별로 나뉘어서 리턴하는데 이유는 모르겠다.
		//unsigned long long GetBufferHash(unsigned int slot) const { return m_ElementsHash << (slot * 16); }
		unsigned long long GetElementsHash() const { return m_ElementsHash; }

		bool IsDynamic() const { return m_bDynamic; }

	private:
		bool create(const void* pData);

	private:
		Graphics* m_pGraphics = nullptr;
		unsigned char* m_pData = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
		unsigned int m_Count = 0;
		unsigned int m_Stride = 0;
		std::vector<VertexElement> m_Elements;
		unsigned long long m_ElementsHash = 0;
		bool m_bDynamic = false;
	};

	template<typename T> bool VertexBuffer::Create(const std::vector<T>& vertices)
	{
		m_bDynamic = false;
		m_Stride = static_cast<unsigned int>(sizeof(T));
		m_Count = static_cast<unsigned int>(vertices.size());

		return create(static_cast<const void*>(vertices.data()));
	}

	template<typename T> bool VertexBuffer::CreateDynamic(unsigned int count)
	{
		m_bDynamic = true;
		m_Stride = static_cast<unsigned int>(sizeof(T));
		m_Count = count;

		return create(nullptr);
	}
}