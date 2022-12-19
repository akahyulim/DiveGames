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

		bool SetSize(uint32_t vertexCount, const std::vector<VertexElement>& elements, bool bDynamic = false);
		bool SetData(void* data);

		void* Map();
		void Unmap();

		uint8_t* GetData() const { return m_pData; }

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }

		uint32_t GetCount() const { return m_Count; }
		uint32_t GetSize() const { return m_Stride; }
		
		const std::vector<VertexElement>& GetElements() const { return m_Elements; }
		
		uint64_t GetElementsHash() const { return m_ElementsHash; }

		bool IsDynamic() const { return m_bDynamic; }

	private:
		Graphics* m_pGraphics;
		uint8_t* m_pData;
		ID3D11Buffer* m_pBuffer;
		uint32_t m_Count;
		uint32_t m_Stride;
		std::vector<VertexElement> m_Elements;
		uint64_t m_ElementsHash;
		bool m_bDynamic;
	};
}