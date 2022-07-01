#pragma once

namespace Dive
{
	class DvIndexBuffer
	{
	public:
		DvIndexBuffer() = default;
		~DvIndexBuffer();

		bool SetSize(unsigned int count, bool bLargeSize, bool bDynamic = false);

		unsigned char* GetSubResource() { return &m_pData[0]; }

		unsigned int GetCount() const { return m_Count; }

	private:
		bool create();

	private:
		ID3D11Buffer* m_pBuffer = nullptr;

		unsigned int m_Count = 0;
		unsigned int m_Size = 0;
		bool m_bDynamic = false;

		unsigned char* m_pData = nullptr;
	};
}