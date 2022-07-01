#include "divepch.h"
#include "DvIndexBuffer.h"
#include "Base/Base.h"
#include "Renderer/Renderer.h"
#include "GraphicsDevice.h"

namespace Dive
{
	DvIndexBuffer::~DvIndexBuffer()
	{
		if (m_pData)
			delete[] m_pData;
	}

	bool DvIndexBuffer::SetSize(unsigned int count, bool bLargeSize, bool bDynamic)
	{
		m_Count = count;
		m_Size = bLargeSize ? (unsigned int)sizeof(unsigned long) : (unsigned int)sizeof(unsigned short);
		m_bDynamic = bDynamic;

		// shadow data? => 크기를 설정해 주어야 한다.
		// 결국 1바이트짜리를 많이 만드는 것인데... 그리고 다시 타입을 변환(unsigned short or unsigned long)해서 저장하는 거다...
		m_pData = new unsigned char[m_Count * m_Size];

		return create();
	}

	bool DvIndexBuffer::create()
	{
		// 함수화하기(Release? Destroy?)
		DV_RELEASE(m_pBuffer);

		if (!m_Count)
			return true;

		// 이걸 어떻게 가지게 할 것인가...
		auto graphics = Renderer::GetGraphicsDevice();
		
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.ByteWidth = (UINT)(m_Count * m_Size);

		if (FAILED(graphics.GetDevice()->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_pBuffer)))
		{
			DV_RELEASE(m_pBuffer);
			DV_CORE_ERROR("인덱스 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}