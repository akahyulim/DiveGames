#pragma once

namespace Dive
{
	class DvIndexBuffer
	{
	public:
		DvIndexBuffer() = default;
		~DvIndexBuffer();

		bool SetSize(unsigned int count, bool bLargeSize, bool bDynamic = false);

		unsigned char* GetData() { return &m_pData[0]; }

		unsigned int GetIndexCount() const { return m_IndexCount; }
		unsigned int GetIndexSize() const { return m_IndexSize; }

	private:
		bool create();

	private:
		ID3D11Buffer* m_pBuffer = nullptr;

		unsigned int m_IndexCount = 0;
		unsigned int m_IndexSize = 0;
		bool m_bDynamic = false;

		unsigned char* m_pData = nullptr;
	};
}