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
		m_IndexCount = count;
		m_IndexSize = bLargeSize ? (unsigned int)sizeof(unsigned long) : (unsigned int)sizeof(unsigned short);
		m_bDynamic = bDynamic;

		// shadow data? => 크기를 설정해 주어야 한다.
		// 결국 1바이트 * count * size이다. 특별한 어려움은 없다.
		m_pData = new unsigned char[m_IndexCount * m_IndexSize];
		DV_ASSERT(m_pData);

		return create();
	}

	// 기본으로 m_bDynamic은 false다.
	// 그렇다면 map / unmap은 어떻게 되는 것일까?
	// => 찾아보니 map / unmap이 아니다. 다른 버퍼에 저장하는 듯 하다. 복잡하다.
	bool DvIndexBuffer::create()
	{
		// 함수화하기(Release? Destroy?)
		DV_RELEASE(m_pBuffer);

		if (!m_IndexCount)
			return true;

		// 이걸 어떻게 가지게 할 것인가...
		auto graphics = Renderer::GetGraphicsDevice();
		
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.ByteWidth = m_IndexCount * m_IndexSize;

		if (FAILED(graphics.GetDevice()->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_pBuffer)))
		{
			DV_RELEASE(m_pBuffer);
			DV_CORE_ERROR("인덱스 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}