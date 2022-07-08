#pragma once
#include "GraphicsDef.h"

namespace Dive
{
	class DvVertexBuffer
	{
	public:
		DvVertexBuffer() = default;
		~DvVertexBuffer();

		// typeµµ Àü´Þ
		bool SetSize(unsigned int count, const std::vector<VertexElement>& elements, bool bDynamic = false);

		unsigned int GetVertexCount() const { return m_VertexCount; }
		unsigned int GetVertexSize() const { return m_VertexSize; }

		unsigned char* GetData() { return m_pData;}

	private:
		bool create();

	private:
		ID3D11Buffer* m_pBuffer = nullptr;

		std::vector<VertexElement> m_Elements;

		eVertexType m_VertexType = eVertexType::Static;

		unsigned int m_VertexCount = 0;
		unsigned int m_VertexSize = 0;

		bool m_bDynamic = false;

		unsigned char* m_pData = nullptr;
	};
}