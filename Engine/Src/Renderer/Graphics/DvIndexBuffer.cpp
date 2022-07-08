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

		// shadow data? => ũ�⸦ ������ �־�� �Ѵ�.
		// �ᱹ 1����Ʈ * count * size�̴�. Ư���� ������� ����.
		m_pData = new unsigned char[m_IndexCount * m_IndexSize];
		DV_ASSERT(m_pData);

		return create();
	}

	// �⺻���� m_bDynamic�� false��.
	// �׷��ٸ� map / unmap�� ��� �Ǵ� ���ϱ�?
	// => ã�ƺ��� map / unmap�� �ƴϴ�. �ٸ� ���ۿ� �����ϴ� �� �ϴ�. �����ϴ�.
	bool DvIndexBuffer::create()
	{
		// �Լ�ȭ�ϱ�(Release? Destroy?)
		DV_RELEASE(m_pBuffer);

		if (!m_IndexCount)
			return true;

		// �̰� ��� ������ �� ���ΰ�...
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
			DV_CORE_ERROR("�ε��� ���� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}
}