#include "divepch.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	IndexBuffer::IndexBuffer()
		: m_pBuffer(nullptr)
		, m_Count(0)
	{
	}

	IndexBuffer::IndexBuffer(uint16_t* pIndices, uint32_t count)
		: m_Count(count)
		, m_Format(DXGI_FORMAT_R16_UINT)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(uint16_t) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if(FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer ������ �����Ͽ����ϴ�.");
	}

	IndexBuffer::IndexBuffer(uint32_t* pIndices, uint32_t count)
		: m_Count(count)
		, m_Format(DXGI_FORMAT_R32_UINT)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(uint32_t) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if(FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer ������ �����Ͽ����ϴ�.");
	}

	IndexBuffer::~IndexBuffer()
	{
		Destroy();
	}

	bool IndexBuffer::SetSize(uint32_t count, bool dynamic)
	{
		// urho3d�� �ִ� �޼����.
		// count, size, dynamic ���θ� �����ϰ� create �Լ��� ȣ���Ѵ�.
		// create�Լ��� �ϴ� ���۸� �����Ѵ�.

		return false;
	}
	// �ܺο��� ���۸� ������ �� Mesh�� ������ ���̶��
	// ���� �� SetSize+SetData �޺��� ����� �ʿ�� ���� �� ����.
	bool IndexBuffer::SetData(const void* pData)
	{
		// ������ ���ۿ� �����͸� ������ �ϼ��Ѵ�.
		// dynamic�̶�� map / unmap����,
		// �ƴ϶�� UpdateSubResource�� ó���Ѵ�.

		return false;
	}

	bool IndexBuffer::CreateBuffer(const void* pData, uint32_t count)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = (count > 65535 ? sizeof(uint32_t) : sizeof(uint16_t)) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
		{
			DV_ERROR("IndexBuffer ������ �����Ͽ����ϴ�.");
			return false;
		}

		m_Count = count;

		return true;
	}

	void IndexBuffer::Destroy()
	{
		DV_RELEASE(m_pBuffer);
	}

	IndexBuffer* IndexBuffer::Create(uint16_t* pIndices, uint32_t count)
	{
		return new IndexBuffer(pIndices, count);
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* pIndices, uint32_t count)
	{
		return new IndexBuffer(pIndices, count);
	}
}