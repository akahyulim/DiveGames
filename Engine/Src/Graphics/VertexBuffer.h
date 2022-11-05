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

		void* Map();
		void Unmap();

		unsigned char* GetData() const { return m_pData; }

		ID3D11Buffer* GetBuffer() const { return m_pBuffer.Get(); }
		unsigned int GetVertexCount() const { return m_VertexCount; }
		unsigned int GetVertexSize() const { return m_VertexSize; }
		const std::vector<VertexElement>& GetElements() const { return m_Elements; }
		unsigned long long GetElementHash() const { return m_ElementHash; }
		unsigned long long GetBufferHash(unsigned int slot) const { return m_ElementHash << (slot * 16); }
		bool IsDynamic() const { return m_bDynamic; }

	private:
		Graphics* m_pGraphics = nullptr;
		unsigned char* m_pData = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
		unsigned int m_VertexCount = 0;
		unsigned int m_VertexSize = 0;
		std::vector<VertexElement> m_Elements;
		unsigned long long m_ElementHash = 0;
		bool m_bDynamic = false;
	};
}